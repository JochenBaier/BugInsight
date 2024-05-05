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

//Based on VLD, Copyright © 2005-2017 VLD Team
//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#include "iat_helper.hpp"

#include <cassert>

#include "getprocaddress_monitor.hpp"
#include "log.hpp"
#include "patched_functions.hpp"

namespace iat_helper
{
void patch_function(const moduleinfo_t& p_module_info, const bool& p_revert_patch, IMAGE_THUNK_DATA* p_thunk, LPVOID p_func, const std::vector<function_to_patch_t>::iterator& p_found_iter,
  const function_to_patch_t& p_function_to_path)
{
  DWORD protect{};
  if(!VirtualProtect(&p_thunk->u1.Function, sizeof(p_thunk->u1.Function), PAGE_EXECUTE_READWRITE, &protect))
  {
    assert(false);
    return;
  }

  if(p_revert_patch)
  {
    p_thunk->u1.Function=(DWORD_PTR)*p_function_to_path.m_orginal;
  } else
  {
    p_thunk->u1.Function=(DWORD_PTR)p_function_to_path.m_replacement;
    assert(p_func);
    *p_found_iter->m_orginal=p_func;
  }

  if(!VirtualProtect(&p_thunk->u1.Function, sizeof(p_thunk->u1.Function), protect, &protect))
  {
    assert(false);
    return;
  }

  if(p_revert_patch)
  {
    LOG(debug, "IAT: Loaded module '{}', unpatched function: '{}' [{}->{}] (from module '{}')", p_module_info.name, p_found_iter->m_function_name,
      (void*)p_found_iter->m_replacement, (void*)p_found_iter->m_orginal, p_found_iter->m_module_name);
  } else
  {
    LOG(debug, "IAT: Loaded module '{}', patched function: '{}' [{}->{}] (from module '{}')", p_module_info.name, p_found_iter->m_function_name,
      (void*)p_found_iter->m_replacement, (void*)p_found_iter->m_orginal, p_found_iter->m_module_name);
  }
}

LPVOID find_real_code(LPVOID pCode)
{
  if(!pCode)
  {
    return pCode;
  }

  LPVOID result=pCode;
  if(*(WORD*)pCode == 0x25ff) // JMP r/m32
  {
#ifdef _WIN64
    LONG offset=*(LONG*)((ULONG_PTR)pCode + 2);
    // RIP relative addressing
    PBYTE pNextInst=(PBYTE)((ULONG_PTR)pCode + 6);
    pCode=*(LPVOID*)(pNextInst + offset);
    return pCode;
#else
    DWORD addr=*((DWORD*)((ULONG_PTR)pCode + 2));
    pCode=*(LPVOID*)(addr);
    return find_real_code(pCode);
#endif
  }
  if(*(BYTE*)pCode == 0xE9) // JMP rel32
  {
    // Relative next instruction
    PBYTE	pNextInst=(PBYTE)((ULONG_PTR)pCode + 5);
    LONG	offset=*(LONG*)((ULONG_PTR)pCode + 1);
    pCode=(LPVOID*)(pNextInst + offset);
    return find_real_code(pCode);
  }
  return result;
}

}

