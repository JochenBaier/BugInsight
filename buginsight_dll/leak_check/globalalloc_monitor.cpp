// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License.  

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

