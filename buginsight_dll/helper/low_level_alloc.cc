// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//This program is free software:you can redistribute it and /or modify it under the terms of the GNU
//General Public License as published by the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
//even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//See the GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License along with this program.If not, see
//<https://www.gnu.org/licenses/>. 

//Copyright 2017 The Abseil Authors (Apache-2.0)
//Copyright 2023 Jochen Baier <email@jochen-baier.de>


// LowLevelAlloc requires that the platform support low-level
// allocation of virtual memory. Platforms lacking this cannot use
// LowLevelAlloc.

#include "low_level_alloc.h"

#include <type_traits>
#include <cassert>

#ifndef _WIN32
#include <pthread.h>
#include <signal.h>
#include <sys/mman.h>
#include <unistd.h>
#else
#include <windows.h>
#endif

#ifdef __linux__
#include <sys/prctl.h>
#endif

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <new>  // for placement-new
#include <cstring>
#include <boost/core/ignore_unused.hpp>

#include "attributes.hpp"

#if defined(MAP_ANON) && !defined(MAP_ANONYMOUS)
#define MAP_ANONYMOUS MAP_ANON
#endif

// A first-fit allocator with amortized logarithmic free() time.

// ---------------------------------------------------------------------------
static const int kMaxLevel=30;

namespace
{
// This struct describes one allocated block, or one free block.
struct AllocList
{
  struct Header
  {
    // Size of entire region, including this field. Must be
    // first. Valid in both allocated and unallocated blocks.
    uintptr_t size;

    // kMagicAllocated or kMagicUnallocated xor this.
    uintptr_t magic;

    // Pointer to parent arena.
    LowLevelAlloc::Arena* arena;

    // Aligns regions to 0 mod 2*sizeof(void*).
    void* dummy_for_alignment;
  } header;

  // Next two fields: in unallocated blocks: freelist skiplist data
  //                  in allocated blocks: overlaps with client data

  // Levels in skiplist used.
  int levels;

  // Actually has levels elements. The AllocList node may not have room
  // for all kMaxLevel entries. See max_fit in LLA_SkiplistLevels().
  AllocList* next[kMaxLevel];
};
}  // namespace

// ---------------------------------------------------------------------------
// A trivial skiplist implementation.  This is used to keep the freelist
// in address order while taking only logarithmic time per insert and delete.

// An integer approximation of log2(size/base)
// Requires size >= base.
static int IntLog2(size_t size, size_t base)
{
  int result=0;
  for(size_t i=size; i > base; i>>=1) {  // i == floor(size/2**result)
    result++;
  }
  //    floor(size / 2**result) <= base < floor(size / 2**(result-1))
  // =>     log2(size/(base+1)) <= result < 1+log2(size/base)
  // => result ~= log2(size/base)
  return result;
}

// Return a random integer n:  p(n)=1/(2**n) if 1 <= n; p(n)=0 if n < 1.
static int Random(uint32_t* state)
{
  uint32_t r=*state;
  int result=1;
  while((((r=r * 1103515245 + 12345) >> 30) & 1) == 0) {
    result++;
  }
  *state=r;
  return result;
}

// Return a number of skiplist levels for a node of size bytes, where
// base is the minimum node size.  Compute level=log2(size / base)+n
// where n is 1 if random is false and otherwise a random number generated with
// the standard distribution for a skiplist:  See Random() above.
// Bigger nodes tend to have more skiplist levels due to the log2(size / base)
// term, so first-fit searches touch fewer nodes.  "level" is clipped so
// level<kMaxLevel and next[level-1] will fit in the node.
// 0 < LLA_SkiplistLevels(x,y,false) <= LLA_SkiplistLevels(x,y,true) < kMaxLevel
static int LLA_SkiplistLevels(size_t size, size_t base, uint32_t* random)
{
  // max_fit is the maximum number of levels that will fit in a node for the
  // given size.   We can't return more than max_fit, no matter what the
  // random number generator says.
  const size_t max_fit=(size - offsetof(AllocList, next)) / sizeof(AllocList*);
  int level=IntLog2(size, base) + (random != nullptr?Random(random):1);
  if(static_cast<size_t>(level) > max_fit) level=static_cast<int>(max_fit);
  if(level > kMaxLevel - 1) level=kMaxLevel - 1;
  assert(level >= 1); //"block not big enough for even one level";
  return level;
}

// Return "atleast", the first element of AllocList *head s.t. *atleast >= *e.
// For 0 <= i < head->levels, set prev[i] to "no_greater", where no_greater
// points to the last element at level i in the AllocList less than *e, or is
// head if no such element exists.
static AllocList* LLA_SkiplistSearch(AllocList* head, const AllocList* e,
  AllocList** prev)
{
  AllocList* p=head;
  for(int level=head->levels - 1; level >= 0; level--) {
    for(AllocList* n; (n=p->next[level]) != nullptr && n < e; p=n) {
    }
    prev[level]=p;
  }
  return (head->levels == 0)?nullptr:prev[0]->next[0];
}

// Insert element *e into AllocList *head.  Set prev[] as LLA_SkiplistSearch.
// Requires that e->levels be previously set by the caller (using
// LLA_SkiplistLevels())
static void LLA_SkiplistInsert(AllocList* head, AllocList* e,
  AllocList** prev)
{
  LLA_SkiplistSearch(head, e, prev);
  for(; head->levels < e->levels; head->levels++) {  // extend prev pointers
    prev[head->levels]=head;                        // to all *e's levels
  }
  for(int i=0; i != e->levels; i++) {  // add element to list
    e->next[i]=prev[i]->next[i];
    prev[i]->next[i]=e;
  }
}

// Remove element *e from AllocList *head.  Set prev[] as LLA_SkiplistSearch().
// Requires that e->levels be previous set by the caller (using
// LLA_SkiplistLevels())
static void LLA_SkiplistDelete(AllocList* head, AllocList* e,
  AllocList** prev)
{
  const AllocList* found=LLA_SkiplistSearch(head, e, prev);
  assert(e == found); //"element not in freelist";
  UNUSED(e)
    UNUSED(found)
    for(int i=0; i != e->levels && prev[i]->next[i] == e; i++) {
      prev[i]->next[i]=e->next[i];
    }
  while(head->levels > 0 && head->next[head->levels - 1] == nullptr) {
    head->levels--;  // reduce head->levels if level unused
  }
}

// ---------------------------------------------------------------------------

// Arena implementation

// Metadata for an LowLevelAlloc arena instance.
struct LowLevelAlloc::Arena
{
  // Constructs an arena with the given LowLevelAlloc flags.
  explicit Arena(uint32_t flags_value);

  // Head of free list, sorted by address
  AllocList freelist;
  // Count of allocated blocks
  int32_t allocation_count;
  // flags passed to NewArena
  const uint32_t flags;
  // Result of sysconf(_SC_PAGESIZE)
  const size_t pagesize;
  // Lowest power of two >= max(16, sizeof(AllocList))
  const size_t round_up;
  // Smallest allocation block size
  const size_t min_size;
  // PRNG state
  uint32_t random;
};

namespace
{
// Static storage space for the lazily-constructed, default global arena
// instances.  We require this space because the whole point of LowLevelAlloc
// is to avoid relying on malloc/new.
alignas(LowLevelAlloc::Arena) unsigned char unhooked_arena_storage[sizeof(
  LowLevelAlloc::Arena)];


void CreateGlobalArenas()
{
  new (&unhooked_arena_storage) LowLevelAlloc::Arena(0);
}

// Returns a global arena that does not call into hooks.  Used by NewArena()
// when kCallMallocHook is not set.
LowLevelAlloc::Arena* UnhookedArena()
{
  CreateGlobalArenas();
  return reinterpret_cast<LowLevelAlloc::Arena*>(&unhooked_arena_storage);
}

}  // namespace


// magic numbers to identify allocated and unallocated blocks
static const uintptr_t kMagicAllocated=0x4c833e95U;
static const uintptr_t kMagicUnallocated=~kMagicAllocated;

// create an appropriate magic number for an object at "ptr"
// "magic" should be kMagicAllocated or kMagicUnallocated
inline static uintptr_t Magic(uintptr_t magic, AllocList::Header* ptr)
{
  return magic ^ reinterpret_cast<uintptr_t>(ptr);
}

namespace
{
size_t GetPageSize()
{
#ifdef _WIN32
  SYSTEM_INFO system_info;
  GetSystemInfo(&system_info);
  return (std::max)(system_info.dwPageSize, system_info.dwAllocationGranularity);
#elif defined(__wasm__) || defined(__asmjs__)
  return getpagesize();
#else
  return static_cast<size_t>(sysconf(_SC_PAGESIZE));
#endif
}

size_t RoundedUpBlockSize()
{
  // Round up block sizes to a power of two close to the header size.
  size_t round_up=16;
  while(round_up < sizeof(AllocList::Header)) {
    round_up+=round_up;
  }
  return round_up;
}

}  // namespace

LowLevelAlloc::Arena::Arena(uint32_t flags_value)
  :
  allocation_count(0),
  flags(flags_value),
  pagesize(GetPageSize()),
  round_up(RoundedUpBlockSize()),
  min_size(2 * round_up),
  random(0)
{
  freelist.header.size=0;
  freelist.header.magic=Magic(kMagicUnallocated, &freelist.header);
  freelist.header.arena=this;
  freelist.levels=0;
  memset(freelist.next, 0, sizeof(freelist.next));
}

// L < meta_data_arena->mu
LowLevelAlloc::Arena* LowLevelAlloc::NewArena(uint32_t flags)
{
  Arena* meta_data_arena=UnhookedArena();
  Arena* result=
    new (AllocWithArena(sizeof(*result), meta_data_arena)) Arena(flags);
  return result;
}

// L < arena->mu, L < arena->arena->mu
bool LowLevelAlloc::DeleteArena(Arena* arena)
{
  assert(
    arena != nullptr && arena != UnhookedArena());
  //"may not delete default arena";

  if(arena->allocation_count != 0) {

    return false;
  }
  while(arena->freelist.next[0] != nullptr) {
    AllocList* region=arena->freelist.next[0];
    size_t size=region->header.size;
    UNUSED(size)
      arena->freelist.next[0]=region->next[0];
    assert(
      region->header.magic == Magic(kMagicUnallocated, &region->header));
    //"bad magic number in DeleteArena()";
    assert(region->header.arena == arena);
    //"bad arena pointer in DeleteArena()";
    assert(size % arena->pagesize == 0);
    //"empty arena has non-page-aligned block size";
    assert(reinterpret_cast<uintptr_t>(region) % arena->pagesize == 0);
    //"empty arena has non-page-aligned block";
    int munmap_result;
#ifdef _WIN32
    munmap_result=VirtualFree(region, 0, MEM_RELEASE);
    boost::ignore_unused(munmap_result);
    assert(munmap_result != 0);
    //"LowLevelAlloc::DeleteArena: VitualFree failed");
#else
#ifndef ABSL_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
    if((arena->flags & LowLevelAlloc::kAsyncSignalSafe) == 0) {
      munmap_result=munmap(region, size);
    } else {
      assert(false);
    }
#else
    munmap_result=munmap(region, size);
#endif  // ABSL_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
    if(munmap_result != 0) {
      assert(false); //"LowLevelAlloc::DeleteArena: munmap failed: %d",
      //errno);
    }
#endif  // _WIN32
  }

  arena->~Arena();
  Free(arena);
  return true;
}

// ---------------------------------------------------------------------------

// Addition, checking for overflow.  The intent is to die if an external client
// manages to push through a request that would cause arithmetic to fail.
static inline uintptr_t CheckedAdd(uintptr_t a, uintptr_t b)
{
  const uintptr_t sum=a + b;
  assert(sum >= a);// "LowLevelAlloc arithmetic overflow");
  return sum;
}

// Return value rounded up to next multiple of align.
// align must be a power of two.
static inline uintptr_t RoundUp(uintptr_t addr, uintptr_t align)
{
  return CheckedAdd(addr, align - 1) & ~(align - 1);
}

// Equivalent to "return prev->next[i]" but with sanity checking
// that the freelist is in the correct order, that it
// consists of regions marked "unallocated", and that no two regions
// are adjacent in memory (they should have been coalesced).
// L >= arena->mu
static AllocList* Next(int i, AllocList* prev, const LowLevelAlloc::Arena* arena)
{
  assert(i < prev->levels); //"too few levels in Next()");
  AllocList* next=prev->next[i];
  if(next != nullptr) {
    assert(
      next->header.magic == Magic(kMagicUnallocated, &next->header));
    //"bad magic number in Next()");
    assert(next->header.arena == arena); //"bad arena pointer in Next()");
    if(prev != &arena->freelist) {
      assert(prev < next); //"unordered freelist");
      assert(reinterpret_cast<char*>(prev) + prev->header.size <
        reinterpret_cast<char*>(next));
      //"malformed freelist");
    }
  }
  return next;
}

// Coalesce list item "a" with its successor if they are adjacent.
static void Coalesce(AllocList* a)
{
  AllocList* n=a->next[0];
  if(n != nullptr && reinterpret_cast<char*>(a) + a->header.size ==
    reinterpret_cast<char*>(n)) {
    LowLevelAlloc::Arena* arena=a->header.arena;
    a->header.size+=n->header.size;
    n->header.magic=0;
    n->header.arena=nullptr;
    AllocList* prev[kMaxLevel];
    LLA_SkiplistDelete(&arena->freelist, n, prev);
    LLA_SkiplistDelete(&arena->freelist, a, prev);
    a->levels=
      LLA_SkiplistLevels(a->header.size, arena->min_size, &arena->random);
    LLA_SkiplistInsert(&arena->freelist, a, prev);
  }
}

// Adds block at location "v" to the free list
// L >= arena->mu
static void AddToFreelist(void* v, LowLevelAlloc::Arena* arena)
{
  AllocList* f=reinterpret_cast<AllocList*>(static_cast<char*>(v) -
    sizeof(f->header));
  assert(f->header.magic == Magic(kMagicAllocated, &f->header));
  //"bad magic number in AddToFreelist()");
  assert(f->header.arena == arena);
  //"bad arena pointer in AddToFreelist()");
  f->levels=
    LLA_SkiplistLevels(f->header.size, arena->min_size, &arena->random);
  AllocList* prev[kMaxLevel];
  LLA_SkiplistInsert(&arena->freelist, f, prev);
  f->header.magic=Magic(kMagicUnallocated, &f->header);
  Coalesce(f);        // maybe coalesce with successor
  Coalesce(prev[0]);  // maybe coalesce with predecessor
}

// Frees storage allocated by LowLevelAlloc::Alloc().
// L < arena->mu
void LowLevelAlloc::Free(void* v)
{
  if(v != nullptr) {
    const AllocList* f=reinterpret_cast<AllocList*>(static_cast<char*>(v) -
      sizeof(f->header));
    LowLevelAlloc::Arena* arena=f->header.arena;

    AddToFreelist(v, arena);
    assert(arena->allocation_count > 0); //"nothing in arena to free";
    arena->allocation_count--;
  }
}

// allocates and returns a block of size bytes, to be freed with Free()
// L < arena->mu
static void* DoAllocWithArena(size_t request, LowLevelAlloc::Arena* arena)
{
  void* result=nullptr;
  if(request != 0) {
    AllocList* s;  // will point to region that satisfies request

    // round up with header
    const size_t req_rnd=
      RoundUp(CheckedAdd(request, sizeof(s->header)), arena->round_up);
    for(;;) {  // loop until we find a suitable region
      // find the minimum levels that a block of this size must have
      const int i=LLA_SkiplistLevels(req_rnd, arena->min_size, nullptr) - 1;
      if(i < arena->freelist.levels) {        // potential blocks exist
        AllocList* before=&arena->freelist;  // predecessor of s
        while((s=Next(i, before, arena)) != nullptr &&
          s->header.size < req_rnd) {
          before=s;
        }
        if(s != nullptr) {  // we found a region
          break;
        }
      }

      // mmap generous 64K chunks to decrease
      // the chances/impact of fragmentation:
      size_t new_pages_size=RoundUp(req_rnd, arena->pagesize * 16);
      void* new_pages;
#ifdef _WIN32
      new_pages=VirtualAlloc(nullptr, new_pages_size,
        MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
      assert(new_pages != nullptr); //"VirtualAlloc failed";
#else
#ifndef ABSL_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
      if((arena->flags & LowLevelAlloc::kAsyncSignalSafe) != 0) {
        assert(false); //FIXME Linux
        //        new_pages = DirectMmap(nullptr, new_pages_size,
        //            PROT_WRITE|PROT_READ, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
      } else {
        new_pages=mmap(nullptr, new_pages_size, PROT_WRITE | PROT_READ,
          MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
      }
#else
      new_pages=mmap(nullptr, new_pages_size, PROT_WRITE | PROT_READ,
        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
#endif  // ABSL_LOW_LEVEL_ALLOC_ASYNC_SIGNAL_SAFE_MISSING
      if(new_pages == MAP_FAILED) {
        assert(false);
      }

#ifdef __linux__
#if defined(PR_SET_VMA) && defined(PR_SET_VMA_ANON_NAME)
      // Attempt to name the allocated address range in /proc/$PID/smaps on
      // Linux.
      //
      // This invocation of prctl() may fail if the Linux kernel was not
      // configured with the CONFIG_ANON_VMA_NAME option.  This is OK since
      // the naming of arenas is primarily a debugging aid.
      prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, new_pages, new_pages_size,
        "absl");
#endif
#endif  // __linux__
#endif  // _WIN32

      s=static_cast<AllocList*>(new_pages);
      s->header.size=new_pages_size;
      // Pretend the block is allocated; call AddToFreelist() to free it.
      s->header.magic=Magic(kMagicAllocated, &s->header);
      s->header.arena=arena;
      AddToFreelist(&s->levels, arena);  // insert new region into free list
    }
    AllocList* prev[kMaxLevel];
    LLA_SkiplistDelete(&arena->freelist, s, prev);  // remove from free list
    // s points to the first free region that's big enough
    if(CheckedAdd(req_rnd, arena->min_size) <= s->header.size) {
      // big enough to split
      AllocList* n=
        reinterpret_cast<AllocList*>(req_rnd + reinterpret_cast<char*>(s));
      n->header.size=s->header.size - req_rnd;
      n->header.magic=Magic(kMagicAllocated, &n->header);
      n->header.arena=arena;
      s->header.size=req_rnd;
      AddToFreelist(&n->levels, arena);
    }
    s->header.magic=Magic(kMagicAllocated, &s->header);
    assert(s->header.arena == arena);
    arena->allocation_count++;

    result=&s->levels;
  }

  return result;
}

void* LowLevelAlloc::AllocWithArena(size_t request, Arena* arena)
{
  assert(arena != nullptr); //"must pass a valid arena"
  void* result=DoAllocWithArena(request, arena);
  return result;
}

