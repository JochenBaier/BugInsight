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

#include <Windows.h>

#include "log_helper.hpp"
#include "memory_leak_map_with_callstack.hpp"
#include "run_time_options.hpp"
#include "memory_helper.hpp"
#include "log.hpp"

extern user_options_t g_user_options;
extern memory_leak_repository_t* g_memory_leak_repository;
static auto g_process_start_ticks=GetTickCount64();
static std::atomic_uint64_t g_next_heap_cheak_ticks{g_process_start_ticks + 1000 * 5};
extern std::atomic_uint64_t g_monitored_api_calls;

using malloc_t=void* (__cdecl*) (size_t);
using _calloc_dbg_t=void* (__cdecl*) (size_t, size_t, int, const char*, int);
using _malloc_dbg_t=void* (__cdecl*) (size_t, int, const char*, int);
using _realloc_dbg_t=void* (__cdecl*) (void*, size_t, int, const char*, int);
using _recalloc_dbg_t=void* (__cdecl*) (void*, size_t, size_t, int, const char*, int);
using calloc_t=void* (__cdecl*) (size_t, size_t);
using new_t=void* (__cdecl*) (size_t);
using new_dbg_crt_t=void* (__cdecl*) (size_t, int, const char*, int);
using new_dbg_mfc_t=void* (__cdecl*) (size_t, const char*, int);
using realloc_t=void* (__cdecl*) (void*, size_t);
using _recalloc_t=void* (__cdecl*) (void*, size_t, size_t);
using _expand_t=void* (__cdecl*) (void*, size_t);
using _expand_dbg_t=void* (__cdecl*) (void*, size_t, int, char const*, int);
using _getcwd_t=char* (__cdecl*) (char*, int);
using _getdcwd_t=char* (__cdecl*) (int, char*, int);
using _getdcwd_dbg_t=char* (__cdecl*) (int, char*, int, int, char const*, int);
using _getcwd_dbg_t=char* (__cdecl*) (char*, int, int, char const*, int);
using _wgetcwd_t=wchar_t* (__cdecl*) (wchar_t*, int);
using _wgetcwd_dbg_t=wchar_t* (__cdecl*) (wchar_t*, int, int, char const*, int);
using _wgetdcwd_t=wchar_t* (__cdecl*) (int, wchar_t*, int);
using _wgetdcwd_dbg_t=wchar_t* (__cdecl*) (int, wchar_t*, int, int, char const*, int);
using _getcwd_t=char* (__cdecl*) (char*, int);
using _getdcwd_t=char* (__cdecl*) (int, char*, int);
using _wgetcwt_t=wchar_t* (__cdecl*) (wchar_t*, int);
using _wgetdcwd_t=wchar_t* (__cdecl*) (int, wchar_t*, int);
using _tempnam_t=char* (__cdecl*) (char const*, char const*);
using _tempnam_dbg_t=char* (__cdecl*) (char const*, char const*, int, char const*, int);
using _wtempnam_t=wchar_t* (__cdecl*) (wchar_t const*, wchar_t const*);
using _wtempnam_dbg_t=wchar_t* (__cdecl*) (wchar_t const*, wchar_t const*, int, char const*, int);
using _dupenv_s_t=errno_t(__cdecl*) (char**, size_t*, char const*);
using _dupenv_s_dbg_t=errno_t(__cdecl*) (char**, size_t*, char const*, int, char const*, int);
using _wdupenv_s_t=errno_t(__cdecl*) (wchar_t**, size_t*, wchar_t const*);
using _wdupenv_s_dbg_t=errno_t(__cdecl*) (wchar_t**, size_t*, wchar_t const*, int, char const*, int);
using _fullpath_t=char* (__cdecl*) (char*, char const*, size_t);
using _fullpath_dbg_t=char* (__cdecl*) (char*, char const*, size_t, int, char const*, int);
using _wfullpath_t=wchar_t* (__cdecl*) (wchar_t*, wchar_t const*, size_t);
using _wfullpath_dbg_t=wchar_t* (__cdecl*) (wchar_t*, wchar_t const*, size_t, int, char const*, int);
using free_t=void(__cdecl*) (void*);
using _free_dbg_t=void(__cdecl*) (void*, int);

inline void check_heap_periodic()
{
  const auto current_ticks=GetTickCount64();
  if(current_ticks < g_next_heap_cheak_ticks)
  {
    return;
  }
  static std::atomic<bool> lock_={false};
  if(lock_.exchange(true, std::memory_order_acquire))
  {
    return;
  }

  LOG(trace, "check heap periodic\n");

  const int res=_heapchk();
  if(res != _HEAPOK)
  {
    auto log_text=fmt::memory_buffer();
    log_helper::build_heap_check_failed_text(std::this_thread::get_id(), "_heapchk", log_text);
    log_helper::process_error(log_text, g_user_options, false);
  }
  const bool vok=HeapValidate(GetProcessHeap(), 0, nullptr);
  if(!vok)
  {
    auto log_text=fmt::memory_buffer();
    log_helper::build_heap_check_failed_text(std::this_thread::get_id(), "HeapValidate", log_text);
    log_helper::process_error(log_text, g_user_options, false);
  }
  const auto current_ticks_fresh=GetTickCount64();
  const auto ms_since_proces_start=current_ticks_fresh - g_process_start_ticks;
  const auto wait_time_ms=((ms_since_proces_start < 60 * 1000)?1000 * 10:1000 * 60 * 5);
  g_next_heap_cheak_ticks=current_ticks_fresh + wait_time_ms;
  lock_.store(false, std::memory_order_release);
}

template<bool debug>
class malloc_monitor_t
{
public:

  inline static malloc_t malloc_fptr{};
  static void* bi_malloc(size_t size)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    void* p=malloc_fptr(size);
    if(!p)
    {
      return p;
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_malloc, api_functions_t::e_malloc);
    return p;
  }

  inline static _malloc_dbg_t _malloc_dbg_fptr{};
  static void* bi__malloc_dbg(size_t size, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    void* p=_malloc_dbg_fptr(size, blockType, filename, linenumber);
    if(!p)
    {
      return p;
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_malloc, api_functions_t::e__malloc_dbg);
    return p;
  }

  inline static calloc_t calloc_fptr{};
  static void* bi_calloc(size_t num, size_t size)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    void* p=calloc_fptr(num, size);
    if(!p)
    {
      return p;
    }

    g_memory_leak_repository->add((uintptr_t)p, num * size, api_family_t::e_malloc, api_functions_t::e_calloc);
    return p;
  }

  inline static _calloc_dbg_t _calloc_dbg_fptr{};
  static void* bi__calloc_dbg(size_t num, size_t size, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    void* p=_calloc_dbg_fptr(num, size, blockType, filename, linenumber);
    if(!p)
    {
      return p;
    }

    g_memory_leak_repository->add((uintptr_t)p, num * size, api_family_t::e_malloc, api_functions_t::e__calloc_dbg);
    return p;
  }

  inline static realloc_t realloc_fptr{};
  static void* bi_realloc(void* memblock, size_t size)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_malloc, api_functions_t::e_realloc);
    }

    void* p=realloc_fptr(memblock, size);
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
    if(found_address)
    {
      g_memory_leak_repository->erase((uintptr_t)memblock);
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_malloc, api_functions_t::e_realloc);
    return p;
  }

  inline static _realloc_dbg_t _realloc_dbg_fptr{};
  static void* bi__realloc_dbg(void* _Block, size_t newSize, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    bool found_address=false;

    if(_Block)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, _Block, api_family_t::e_malloc, api_functions_t::e__realloc_dbg);
    }

    void* p=_realloc_dbg_fptr(_Block, newSize, blockType, filename, linenumber);
    if(!p)
    {
      if(_Block && newSize == 0) //like free(memblock)
      {
        if(found_address)
        {
          g_memory_leak_repository->erase((uintptr_t)_Block);
        }
      }

      return p;
    }

    //delete old pointer
    if(_Block)
    {
      if(found_address)
      {
        g_memory_leak_repository->erase((uintptr_t)_Block);
      }
    }

    g_memory_leak_repository->add((uintptr_t)p, newSize, api_family_t::e_malloc, api_functions_t::e__realloc_dbg);
    return p;
  }

  inline static _recalloc_t _recalloc_fptr{};
  static void* bi__recalloc(void* memblock, size_t num, size_t size)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_malloc, api_functions_t::e__recalloc);
      assert(_CrtIsValidHeapPointer(memblock));
    }

    void* p=_recalloc_fptr(memblock, num, size);
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

    g_memory_leak_repository->add((uintptr_t)p, num * size, api_family_t::e_malloc, api_functions_t::e__recalloc);
    return p;
  }

  inline static _recalloc_dbg_t _recalloc_dbg_fptr{};
  static void* bi__recalloc_dbg(void* memblock, size_t num, size_t size, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_malloc, api_functions_t::e__recalloc_dbg);
      assert(_CrtIsValidHeapPointer(memblock));
    }

    void* p=_recalloc_dbg_fptr(memblock, num, size, blockType, filename, linenumber);
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

    g_memory_leak_repository->add((uintptr_t)p, num * size, api_family_t::e_malloc, api_functions_t::e__recalloc_dbg);
    return p;
  }

  inline static _expand_t _expand_fptr{};
  static void* bi__expand(void* memblock, size_t size)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(memblock)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, memblock, api_family_t::e_malloc, api_functions_t::e__expand);
    }

    void* p=_expand_fptr(memblock, size);
    if(!p)
    {
      return p;
    }

    assert(p == memblock);

    //erase and add with new size
    if(found_address)
    {
      g_memory_leak_repository->erase((uintptr_t)memblock);
    }

    g_memory_leak_repository->add((uintptr_t)p, size, api_family_t::e_malloc, api_functions_t::e__expand);
    return p;
  }

  inline static _expand_dbg_t _expand_dbg_fptr{};
  static void* bi__expand_dbg(void* userData, size_t newSize, int blockType, const char* filename, int lineNumber)
  {
    ++g_monitored_api_calls;

    bool found_address=false;

    if(userData)
    {
      found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, userData, api_family_t::e_malloc, api_functions_t::e__expand_dbg);
      assert(_CrtIsValidHeapPointer(userData));
    }

    void* p=_expand_dbg_fptr(userData, newSize, blockType, filename, lineNumber);
    if(!p)
    {
      return p;
    }

    assert(p == userData);

    //erase and add with new size
    if(found_address)
    {
      g_memory_leak_repository->erase((uintptr_t)userData);
    }

    g_memory_leak_repository->add((uintptr_t)p, newSize, api_family_t::e_malloc, api_functions_t::e__expand_dbg);
    return p;
  }

  inline static _getcwd_t _getcwd_fptr{};
  static char* bi__getcwd(char* buffer, int maxlen)
  {
    ++g_monitored_api_calls;

    char* c=_getcwd_fptr(buffer, maxlen);
    if(!c || buffer != nullptr)
    {
      return c;
    }

    const size_t length=strlen(c);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)c, length + 1, api_family_t::e_malloc, api_functions_t::e__getcwd);
    return c;
  }

  inline static _getcwd_dbg_t _getcwd_dbg_fptr{};
  static char* bi__getcwd_dbg(char* buffer, int maxlen, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    char* c=_getcwd_dbg_fptr(buffer, maxlen, blockType, filename, linenumber);
    if(!c || buffer != nullptr)
    {
      return c;
    }

    const size_t length=strlen(c);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)c, length + 1, api_family_t::e_malloc, api_functions_t::e__getcwd_dbg);
    return c;
  }

  inline static _wgetcwd_t _wgetcwd_fptr{};
  static wchar_t* _wgetcwd(wchar_t* buffer, int maxlen)
  {
    ++g_monitored_api_calls;

    wchar_t* c=_wgetcwd_fptr(buffer, maxlen);
    if(!c || buffer != nullptr)
    {
      return c;
    }

    const size_t char_count=wcslen(c);
    assert(char_count);

    g_memory_leak_repository->add((uintptr_t)c, (char_count + 1) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wgetcwd);
    return c;
  }

  inline static _wgetcwd_dbg_t _wgetcwd_dbg_fptr{};
  static wchar_t* bi__wgetcwd_dbg(wchar_t* buffer, int maxlen, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    wchar_t* c=_wgetcwd_dbg_fptr(buffer, maxlen, blockType, filename, linenumber);
    if(!c || buffer != nullptr)
    {
      return c;
    }

    const size_t char_count=wcslen(c);
    assert(char_count);

    g_memory_leak_repository->add((uintptr_t)c, (char_count + 1) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wgetcwd_dbg);
    return c;
  }

  inline static _getdcwd_t _getdcwd_fptr{};
  static char* bi__getdcwd(int drive, char* buffer, int maxlen)
  {
    ++g_monitored_api_calls;

    char* c=_getdcwd_fptr(drive, buffer, maxlen);
    if(!c || buffer != nullptr)
    {
      return c;
    }

    const size_t length=strlen(c);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)c, length + 1, api_family_t::e_malloc, api_functions_t::e__getdcwd);
    return c;
  }

  inline static _getdcwd_dbg_t _getdcwd_dbg_ftpr{};
  static char* bi__getdcwd_dbg(int drive, char* buffer, int maxlen, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    char* c=_getdcwd_dbg_ftpr(drive, buffer, maxlen, blockType, filename, linenumber);
    if(!c || buffer != nullptr)
    {
      return c;
    }

    const size_t length=strlen(c);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)c, length + 1, api_family_t::e_malloc, api_functions_t::e__getdcwd_dbg);
    return c;
  }

  inline static _wgetdcwd_t _wgetdcwd_fptr{};
  static wchar_t* bi__wgetdcwd(int drive, wchar_t* buffer, int maxlen)
  {
    ++g_monitored_api_calls;

    wchar_t* c=_wgetdcwd_fptr(drive, buffer, maxlen);
    if(!c || buffer != nullptr)
    {
      return c;
    }

    const size_t char_count=wcslen(c);
    assert(char_count);

    g_memory_leak_repository->add((uintptr_t)c, (char_count + 1) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wgetdcwd);
    return c;
  }

  inline static _wgetdcwd_dbg_t _wgetdcwd_dbg_fptr{};
  static wchar_t* bi__wgetdcwd_dbg(int drive, wchar_t* buffer, int maxlen, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    wchar_t* c=_wgetdcwd_dbg_fptr(drive, buffer, maxlen, blockType, filename, linenumber);
    if(!c || buffer != nullptr)
    {
      return c;
    }

    const size_t char_count=wcslen(c);
    assert(char_count);

    g_memory_leak_repository->add((uintptr_t)c, (char_count + 1) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wgetdcwd_dbg);
    return c;
  }

  inline static _tempnam_t _tempnam_fptr{};
  static char* bi__tempnam(const char* dir, const char* prefix)
  {
    ++g_monitored_api_calls;

    char* c=_tempnam_fptr(dir, prefix);
    if(!c)
    {
      return c;
    }

    const size_t length=strlen(c);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)c, length + 1, api_family_t::e_malloc, api_functions_t::e__tempnam);
    return c;
  }

  inline static _tempnam_dbg_t _tempnam_dbg_fptr{};
  static char* bi__tempnam_dbg(const char* dir, const char* prefix, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    char* c=_tempnam_dbg_fptr(dir, prefix, blockType, filename, linenumber);
    if(!c)
    {
      return c;
    }

    const size_t length=strlen(c);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)c, length + 1, api_family_t::e_malloc, api_functions_t::e__tempnam_dbg);
    return c;
  }

  inline static _wtempnam_t _wtempnam_fptr{};
  static wchar_t* bi__wtempnam(const wchar_t* dir, const wchar_t* prefix)
  {
    ++g_monitored_api_calls;

    wchar_t* c=_wtempnam_fptr(dir, prefix);
    if(!c)
    {
      return c;
    }

    const size_t char_count=wcslen(c);
    assert(char_count);

    g_memory_leak_repository->add((uintptr_t)c, (char_count + 1) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wtempnam);
    return c;
  }

  inline static _wtempnam_dbg_t _wtempnam_dbg_fptr{};
  static wchar_t* bi__wtempnam_dbg(const wchar_t* dir, const wchar_t* prefix, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    wchar_t* c=_wtempnam_dbg_fptr(dir, prefix, blockType, filename, linenumber);
    if(!c)
    {
      return c;
    }

    const size_t char_count=wcslen(c);
    assert(char_count);

    g_memory_leak_repository->add((uintptr_t)c, (char_count + 1) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wtempnam_dbg);
    return c;
  }

  inline static _dupenv_s_t _dupenv_s_fptr{};
  static errno_t bi__dupenv_s(char** _Buffer, size_t* _BufferCount, char const* _VarName)
  {
    ++g_monitored_api_calls;

    errno_t err=_dupenv_s_fptr(_Buffer, _BufferCount, _VarName);
    if(err)
    {
      return err;
    }

    g_memory_leak_repository->add((uintptr_t)*_Buffer, *_BufferCount, api_family_t::e_malloc, api_functions_t::e__dupenv_s);
    return err;
  }

  inline static _dupenv_s_dbg_t _dupenv_s_dbg_fptr{};
  static errno_t bi__dupenv_s_dbg(char** _Buffer, size_t* _BufferCount, char const* _VarName, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    errno_t err=_dupenv_s_dbg_fptr(_Buffer, _BufferCount, _VarName, blockType, filename, linenumber);
    if(err)
    {
      return err;
    }

    g_memory_leak_repository->add((uintptr_t)*_Buffer, *_BufferCount, api_family_t::e_malloc, api_functions_t::e__dupenv_s_dbg);
    return err;
  }

  inline static _wdupenv_s_t _wdupenv_s_fptr{};
  static errno_t bi__wdupenv_s(wchar_t** _Buffer, size_t* _BufferCount, wchar_t const* _VarName)
  {
    ++g_monitored_api_calls;

    errno_t err=_wdupenv_s_fptr(_Buffer, _BufferCount, _VarName);
    if(err)
    {
      return err;
    }

    g_memory_leak_repository->add((uintptr_t)*_Buffer, (*_BufferCount) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wdupenv_s);
    return err;
  }

  inline static _wdupenv_s_dbg_t _wdupenv_s_dbg_fptr{};
  static errno_t bi__wdupenv_dbg_s(wchar_t** _Buffer, size_t* _BufferCount, wchar_t const* _VarName, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    errno_t err=_wdupenv_s_dbg_fptr(_Buffer, _BufferCount, _VarName, blockType, filename, linenumber);
    if(err)
    {
      return err;
    }

    g_memory_leak_repository->add((uintptr_t)*_Buffer, (*_BufferCount) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wdupenv_s_dbg);
    return err;
  }

  inline static _fullpath_t _fullpath_fptr{};
  static char* bi__fullpath(char* _Buffer, char const* _Path, size_t _BufferCount)
  {
    ++g_monitored_api_calls;

    char* full=_fullpath_fptr(_Buffer, _Path, _BufferCount);
    if(full == nullptr || _Buffer != nullptr)
    {
      return full;
    }

    const size_t length=strlen(full);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)full, length + 1, api_family_t::e_malloc, api_functions_t::e__fullpath);
    return full;
  }

  inline static _fullpath_dbg_t _fullpath_dbg_fptr{};
  static char* bi__fullpath_dbg(char* _Buffer, char const* _Path, size_t _BufferCount, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    char* full=_fullpath_dbg_fptr(_Buffer, _Path, _BufferCount, blockType, filename, linenumber);
    if(full == nullptr || _Buffer != nullptr)
    {
      return full;
    }

    const size_t length=strlen(full);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)full, length + 1, api_family_t::e_malloc, api_functions_t::e__fullpath_dbg);
    return full;
  }

  inline static _wfullpath_t _wfullpath_fptr{};
  static wchar_t* bi__wfullpath(wchar_t* _Buffer, wchar_t const* _Path, size_t _BufferCount)
  {
    ++g_monitored_api_calls;

    wchar_t* full=_wfullpath_fptr(_Buffer, _Path, _BufferCount);
    if(full == nullptr || _Buffer != nullptr)
    {
      return full;
    }

    const size_t length=wcslen(full);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)full, (length + 1) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wfullpath);
    return full;
  }

  inline static _wfullpath_dbg_t _wfullpath_dbg_fptr{};
  static wchar_t* bi__wfullpath_dbg(wchar_t* _Buffer, wchar_t const* _Path, size_t _BufferCount, int blockType, const char* filename, int linenumber)
  {
    ++g_monitored_api_calls;

    wchar_t* full=_wfullpath_dbg_fptr(_Buffer, _Path, _BufferCount, blockType, filename, linenumber);
    if(full == nullptr || _Buffer != nullptr)
    {
      return full;
    }

    const size_t length=wcslen(full);
    assert(length);

    g_memory_leak_repository->add((uintptr_t)full, (length + 1) * sizeof(wchar_t), api_family_t::e_malloc, api_functions_t::e__wfullpath_dbg);
    return full;
  }

  inline static free_t free_fptr{};
  static void bi_free(void* p_user)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    if(p_user == nullptr)
    {
      free_fptr(p_user);
      return;
    }

    const bool found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, p_user, api_family_t::e_malloc, api_functions_t::e_free);
    if(found_address)
    {
      g_memory_leak_repository->erase((uintptr_t)p_user);
    }

    free_fptr(p_user);

    check_heap_periodic();
  }

  inline static _free_dbg_t _free_dbg_fptr{};
  static void bi__free_dbg(void* userData, int blockType)
  {
    ++g_monitored_api_calls;

    assert(g_memory_leak_repository);

    if(userData == nullptr)
    {
      _free_dbg_fptr(userData, blockType);
      return;
    }

    const bool found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, userData, api_family_t::e_malloc, api_functions_t::e__free_dbg);
    if(found_address)
    {
      g_memory_leak_repository->erase((uintptr_t)userData);
    }

    _free_dbg_fptr(userData, blockType);

    check_heap_periodic();
  }
};

