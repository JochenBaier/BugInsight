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

//Based on VLD, Copyright 2005-2017 VLD Team (LGPL-2.1)
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

