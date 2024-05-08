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


#include "globalalloc_monitor.hpp"

#include <memory_leak_map_with_callstack.hpp>
#include "memory_helper.hpp"
#include "shared_settings.hpp"

extern memory_leak_repository_t* g_memory_leak_repository;
extern std::atomic_uint64_t g_monitored_api_calls;

HLOCAL buginsight_globalalloc(UINT uFlags, SIZE_T uBytes)
{
  ++g_monitored_api_calls;

  HLOCAL p=GlobalAlloc_fptr(uFlags, uBytes);
  if(!p)
  {
    return p;
  }

  g_memory_leak_repository->add((uintptr_t)p, uBytes, api_family_t::e_globalalloc, api_functions_t::e_globalalloc);
  return p;
}

HLOCAL buginsight_globalrealloc(HLOCAL hMem, SIZE_T uBytes, UINT uFlags)
{
  ++g_monitored_api_calls;

  assert(g_memory_leak_repository);

  bool found_address=false;

  if(hMem)
  {
    found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, hMem, api_family_t::e_globalalloc, api_functions_t::e_globrealalloc);
  }

  void* p=::GlobalReAlloc_fptr(hMem, uBytes, uFlags);
  if(!p)
  {
    return p;
  }

  if(found_address)
  {
    g_memory_leak_repository->erase((uintptr_t)hMem);
  }

  g_memory_leak_repository->add((uintptr_t)p, uBytes, api_family_t::e_globalalloc, api_functions_t::e_globrealalloc);
  return p;
}

HLOCAL buginsight_globalfree(HLOCAL hMem)
{
  ++g_monitored_api_calls;

  assert(g_memory_leak_repository);

  if(hMem == nullptr)
  {
    return GlobalFree_fptr(hMem);
  }

  const bool found_address=memory_helper::log_or_abort_if_memory_adddress_is_unknown(*g_memory_leak_repository, hMem, api_family_t::e_globalalloc, api_functions_t::e_globalfree);
  if(found_address)
  {
    g_memory_leak_repository->erase((uintptr_t)hMem);
  }

  return GlobalFree(hMem);
}

