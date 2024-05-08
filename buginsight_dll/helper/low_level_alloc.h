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

// A simple thread-safe memory allocator that does not depend on
// mutexes or thread-specific data.  It is intended to be used
// sparingly, and only when malloc() would introduce an unwanted
// dependency, such as inside the heap-checker, or the Mutex
// implementation.

// IWYU pragma: private, include "base/low_level_alloc.h"

#pragma once

#include <cstdint>
#include <cstddef>

class LowLevelAlloc
{
public:
  struct Arena;       // an arena from which memory may be allocated

  // Returns a pointer to a block of at least "request" bytes
  // that have been newly allocated from the specific arena.
  // for Alloc() call the DefaultArena() is used.
  // Returns 0 if passed request==0.
  // Does not return 0 under other circumstances; it crashes if memory
  // is not available.
  static void* AllocWithArena(size_t request, Arena* arena);

  // Deallocates a region of memory that was previously allocated with
  // Alloc().   Does nothing if passed 0.   "s" must be either 0,
  // or must have been returned from a call to Alloc() and not yet passed to
  // Free() since that call to Alloc().  The space is returned to the arena
  // from which it was allocated.
  static void Free(void* s);

  // ABSL_ATTRIBUTE_SECTION(malloc_hook) for Alloc* and Free
  // are to put all callers of MallocHook::Invoke* in this module
  // into special section,
  // so that MallocHook::GetCallerStackTrace can function accurately.

  // Create a new arena.
  // The root metadata for the new arena is allocated in the
  // meta_data_arena; the DefaultArena() can be passed for meta_data_arena.
  // These values may be ored into flags:
  enum
  {
    // Report calls to Alloc() and Free() via the MallocHook interface.
    // Set in the DefaultArena.
    kCallMallocHook=0x0001,
  };
  // Construct a new arena.  The allocation of the underlying metadata honors
  // the provided flags.  For example, the call NewArena(kAsyncSignalSafe)
  // is itself async-signal-safe, as well as generatating an arena that provides
  // async-signal-safe Alloc/Free.
  static Arena* NewArena(uint32_t flags);

  // Destroys an arena allocated by NewArena and returns true,
  // provided no allocated blocks remain in the arena.
  // If allocated blocks remain in the arena, does nothing and
  // returns false.
  // It is illegal to attempt to destroy the DefaultArena().
  static bool DeleteArena(Arena* arena);

  LowLevelAlloc()=delete;      // no instances
};

