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

#pragma once

#include "memory_helper.hpp"
#include "memory_leak_map_with_callstack.hpp"

extern memory_leak_repository_t* g_memory_leak_repository;
extern std::atomic_uint64_t g_monitored_api_calls;

using _aligned_malloc_t=void* (__cdecl*) (size_t, size_t);
using _aligned_offset_malloc_t=void* (__cdecl*) (size_t, size_t, size_t);
using _aligned_realloc_t=void* (__cdecl*) (void*, size_t, size_t);
using _aligned_offset_realloc_t=void* (__cdecl*) (void*, size_t, size_t, size_t);
using _aligned_recalloc_t=void* (__cdecl*) (void*, size_t, size_t, size_t);
using _aligned_offset_recalloc_t=void* (__cdecl*) (void*, size_t, size_t, size_t, size_t);
using _aligned_malloc_dbg_t=void* (__cdecl*) (size_t, size_t, const char*, int);
using _aligned_offset_malloc_dbg_t=void* (__cdecl*) (size_t, size_t, size_t, const char*, int);
using _aligned_realloc_dbg_t=void* (__cdecl*) (void*, size_t, size_t, const char*, int);
using _aligned_offset_realloc_dbg_t=void* (__cdecl*) (void*, size_t, size_t, size_t, const char*, int);
using _aligned_recalloc_dbg_t=void* (__cdecl*) (void*, size_t, size_t, size_t, const char*, int);
using _aligned_offset_recalloc_dbg_t=void* (__cdecl*) (void*, size_t, size_t, size_t, size_t, const char*, int);
using _aligned_free_t=void(__cdecl*) (void* _Block);;
using _aligned_free_dbg_t=void(__cdecl*) (void* _Block);

template<bool debug>
class _aligned_malloc_monitor_t
{
public:
  inline static _aligned_malloc_t _aligned_malloc_fptr{};
  static void* bi__aligned_malloc(size_t size, size_t alignment)
  {
    ++g_monitored_api_calls;

    void* p=_aligned_malloc_fptr(size, alignment);
    if(!p)
    {
      return p;
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_malloc);
    return p;
  }

  inline static _aligned_malloc_dbg_t _aligned_malloc_dbg_fptr{};
  static void* bi__aligned_malloc_dbg(size_t size, size_t alignment, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    void* p=_aligned_malloc_dbg_fptr(size, alignment, filename, linenumber);
    if(!p)
    {
      return p;
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_malloc_dbg);
    return p;
  }

  inline static _aligned_offset_malloc_t _aligned_offset_malloc_fptr{};
  static void* bi__aligned_offset_malloc(size_t size, size_t alignment, size_t offset)
  {
    ++g_monitored_api_calls;

    void* p=_aligned_offset_malloc_fptr(size, alignment, offset);
    if(!p)
    {
      return p;
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_offset_malloc);
    return p;
  }

  inline static _aligned_offset_malloc_dbg_t _aligned_offset_malloc_dbg_fptr{};
  static void* bi__aligned_offset_malloc_dbg(size_t size, size_t alignment, size_t offset, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    void* p=_aligned_offset_malloc_dbg_fptr(size, alignment, offset, filename, linenumber);
    if(!p)
    {
      return p;
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_offset_malloc_dbg);
    return p;
  }

  inline static _aligned_offset_realloc_t _aligned_offset_realloc_fptr{};
  static void* bi__aligned_offset_realloc(void* memblock, size_t size, size_t alignment, size_t offset)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_offset_realloc);
    }

    void* p=_aligned_offset_realloc_fptr(memblock, size, alignment, offset);
    if(!p)
    {
      if(memblock && size == 0) //like free(memblock) 
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)memblock);
        }
      }

      return p;
    }

    //delete old pointer
    if(memblock)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)memblock);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_offset_realloc);
    return p;
  }

  inline static _aligned_offset_realloc_dbg_t _aligned_offset_realloc_dbg_fptr{};
  static void* bi__aligned_offset_realloc_dbg(void* memblock, size_t size, size_t alignment, size_t offset, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_realloc_dbg);
    }

    void* p=_aligned_offset_realloc_dbg_fptr(memblock, size, alignment, offset, filename, linenumber);
    if(!p)
    {
      if(memblock && size == 0) //like free(memblock)
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)memblock);
        }
      }

      return p;
    }

    //delete old pointer
    if(memblock)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)memblock);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_offset_realloc_dbg);
    return p;
  }

  inline static _aligned_offset_recalloc_t _aligned_offset_recalloc_fptr{};
  static void* bi__aligned_offset_recalloc(void* memblock, size_t num, size_t size, size_t alignment, size_t offset)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_recalloc);
    }

    void* p=_aligned_offset_recalloc_fptr(memblock, num, size, alignment, offset);
    if(!p)
    {
      if(memblock && size == 0) //like free(memblock) 
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)memblock);
        }
      }

      return p;
    }

    //delete old pointer
    if(memblock)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)memblock);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, num * size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_offset_recalloc);
    return p;
  }

  inline static _aligned_offset_recalloc_dbg_t _aligned_offset_recalloc_dbg_fptr{};
  static void* bi__aligned_offset_recalloc_dbg(void* memblock, size_t num, size_t size, size_t alignment, size_t offset, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_recalloc_dbg);
    }

    void* p=_aligned_offset_recalloc_dbg_fptr(memblock, num, size, alignment, offset, filename, linenumber);
    if(!p)
    {
      if(memblock && size == 0) //like free(memblock) 
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)memblock);
        }
      }
      return p;
    }

    //delete old pointer
    if(memblock)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)memblock);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, num * size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_offset_recalloc_dbg);
    return p;
  }

  inline static _aligned_realloc_t _aligned_realloc_fptr{};
  static void* bi__aligned_realloc(void* memblock, size_t size, size_t alignment)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_realloc);
    }

    void* p=_aligned_realloc_fptr(memblock, size, alignment);
    if(!p)
    {
      if(memblock && size == 0) //like free(memblock) 
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)memblock);
        }
      }

      return p;
    }

    //delete old pointer
    if(memblock)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)memblock);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_realloc);
    return p;
  }

  inline static _aligned_realloc_dbg_t _aligned_realloc_dbg_fptr{};
  static void* bi__aligned_realloc_dbg(void* memblock, size_t size, size_t alignment, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_realloc_dbg);
    }

    void* p=_aligned_realloc_dbg_fptr(memblock, size, alignment, filename, linenumber);
    if(!p)
    {
      if(memblock && size == 0) //like free(memblock)
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)memblock);
        }
      }

      return p;
    }

    //delete old pointer
    if(memblock)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)memblock);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_realloc_dbg);
    return p;
  }

  inline static _aligned_recalloc_t _aligned_recalloc_fptr{};
  static void* bi__aligned_recalloc(void* memblock, size_t num, size_t size, size_t alignment)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_recalloc);
    }

    void* p=_aligned_recalloc_fptr(memblock, num, size, alignment);
    if(!p)
    {
      if(memblock && size == 0) //like free(memblock)
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)memblock);
        }
      }

      return p;
    }

    //delete old pointer
    if(memblock)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)memblock);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, num * size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_recalloc);
    return p;
  }

  inline static _aligned_recalloc_dbg_t _aligned_recalloc_dbg_fptr{};
  static void* bi__aligned_recalloc_dbg(void* memblock, size_t num, size_t size, size_t alignment, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_recalloc_dbg);
    }

    void* p=_aligned_recalloc_dbg_fptr(memblock, num, size, alignment, filename, linenumber);
    if(!p)
    {
      if(memblock && size == 0) //like free(memblock)
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)memblock);
        }
      }
      return p;
    }

    //delete old pointer
    if(memblock)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)memblock);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, num * size, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_recalloc_dbg);
    return p;
  }

  inline static _aligned_free_t _aligned_free_fptr{};
  static void bi__aligned_free(void* memblock)
  {
    ++g_monitored_api_calls;

    if(memblock == nullptr)
    {
      _aligned_free_fptr(memblock);
      return;
    }

    const bool found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_free);
    if(found_address)
    {
      g_memory_leak_repository->erase((uintptr_t)memblock);
    }

    _aligned_free_fptr(memblock);
  }

  inline static _aligned_free_dbg_t _aligned_free_dbg_fptr{};
  static void bi__aligned_free_dbg(void* memblock)
  {
    ++g_monitored_api_calls;

    if(memblock == nullptr)
    {
      _aligned_free_dbg_fptr(memblock);
      return;
    }

    const bool found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_aligned_malloc, api_functions_t::e__aligned_free_dbg);
    if(found_address)
    {
      g_memory_leak_repository->erase((uintptr_t)memblock);
    }

    _aligned_free_dbg_fptr(memblock);
  }

};

