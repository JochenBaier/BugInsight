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

//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#include "heap_monitor.hpp"

#include <Windows.h>
#include <werapi.h>

#include "memory_leak_map_with_callstack.hpp"
#include <loader.hpp>
#include "log_helper.hpp"
#include "deadlock_monitor.hpp"

extern user_options_t g_user_options;
extern memory_leak_repository_t* g_memory_leak_repository;
extern std::atomic_uint64_t g_monitored_api_calls;

TM_ATTRIBUTE_NOINLINE bool log_or_abort_if_heapalloc_memory_adddress_is_unknown(HANDLE hHeap, void* _Block, const api_functions_t p_api_function)
{
  api_family_t api_family=api_family_t::e_unknow;
  boost::span<uint8_t> user_memory;
  api_functions_t api_function_stored=api_functions_t::e_unknown;
  const bool found=g_memory_leak_repository->get_data((uintptr_t)hHeap, (uintptr_t)_Block, api_family, user_memory, api_function_stored);
  if(!found)
  {
    auto log_text=fmt::memory_buffer();
    log_helper::build_unknown_memory_address_text(std::this_thread::get_id(), p_api_function, _Block, log_text);
    log_helper::process_error(log_text, g_user_options, false);
    return false;
  }

  assert(api_family == api_family_t::e_heapapi);
  return true;
}

LPVOID buginsight_heapalloc(HANDLE hHeap, DWORD dwFlags, SIZE_T dwBytes)
{
  ++g_monitored_api_calls;
  assert(hHeap);
  LPVOID p=HeapAlloc_fptr(hHeap, dwFlags, dwBytes);
  if(p == nullptr)
  {
    return p;
  }

  g_memory_leak_repository->add((uintptr_t)hHeap, (size_t)p, dwBytes, api_functions_t::e_HeapAlloc);
  return (void*)p;
}

LPVOID buginsight_heaprepalloc(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem, SIZE_T dwBytes)
{
  ++g_monitored_api_calls;
  assert(hHeap);
  assert(lpMem);

  bool found_address=false;

  if(hHeap && lpMem)
  {
    found_address=log_or_abort_if_heapalloc_memory_adddress_is_unknown(hHeap, lpMem, api_functions_t::e_HeapReAlloc);
  }

  const LPVOID p=HeapReAlloc_fptr(hHeap, dwFlags, lpMem, dwBytes);
  if(p == nullptr)
  {
    return p;
  }

  //delete old pointer
  if(found_address)
  {
    g_memory_leak_repository->erase((uintptr_t)hHeap, (uintptr_t)lpMem);
  }

  g_memory_leak_repository->add((uintptr_t)hHeap, (uintptr_t)p, dwBytes, api_functions_t::e_HeapReAlloc);
  return (void*)p;
}

BOOL buginsight_heapfree(HANDLE hHeap, DWORD dwFlags, LPVOID lpMem)
{
  ++g_monitored_api_calls;

  assert(g_memory_leak_repository);
  if(!hHeap || !lpMem)
  {
    return HeapFree(hHeap, dwFlags, lpMem);
  }

  const bool knnow_adddress=log_or_abort_if_heapalloc_memory_adddress_is_unknown(hHeap, lpMem, api_functions_t::e_HeapFree);
  if(knnow_adddress)
  {
    g_memory_leak_repository->erase((uintptr_t)hHeap, (uintptr_t)lpMem);
  }

  const BOOL status=HeapFree_fptr(hHeap, dwFlags, lpMem);
  return status;

}

