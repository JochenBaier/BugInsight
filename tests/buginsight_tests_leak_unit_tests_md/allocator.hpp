#pragma once

#include "metamalloc.h"
#include "simple_heap_pow2.h"
using namespace metamalloc;

using CentralHeapType=SimpleHeapPow2<ConcurrencyPolicy::CENTRAL>;
using LocalHeapType=SimpleHeapPow2<ConcurrencyPolicy::THREAD_LOCAL>;

using AllocatorType=ScalableAllocator< CentralHeapType, LocalHeapType >;

template <class T>
class meta_allocator
{
public:
  using value_type=T;

  meta_allocator()
  {
    int brk=1;
  }

  template <class U>
  meta_allocator(const meta_allocator<U>&) {}

  T* allocate(const std::size_t n)
  {
    T* ret=reinterpret_cast<T*>(AllocatorType::get_instance().allocate(n * sizeof(T)));
    return ret;
  }

  void deallocate(T* const p, const std::size_t n)
  {
    AllocatorType::get_instance().deallocate(p);
  }
};
