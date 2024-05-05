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


#include "iat_patching.hpp"

#include <regex>

#include <boost/algorithm/string/case_conv.hpp>

#include "iat_helper.hpp"
#include "patched_functions.hpp"
#include "shared_settings.hpp"
#include <log.hpp>
#include "scope_exit.hpp"
#include "dbghelp_api.hpp"

#define R2VA(moduleBase, rva)  (((PBYTE)(moduleBase)) + (rva)) // Relative Virtual Address to Virtual Address conversion.

namespace iat_patching
{
void patch_iat(const std::string& p_buginsight_dll_name, const moduleinfo_t& p_module_info, const std::vector<function_type_t>& p_enabled_function_types,
  std::vector<function_to_patch_t>& p_functions_to_patch, const bool& p_revert_patch, bool& p_found_ucrtbase_dll)
{
  assert(p_module_info.module_handle);

  IMAGE_SECTION_HEADER* section=nullptr;
  ULONG size=0;

  // Locate the importing module's Import Directory Table (IDT) entry for the
  // exporting module. The importing module actually can have several IATs --
  // one for each export module that it imports something from. The IDT entry
  // gives us the offset of the IAT for the module we are interested in.
  const IMAGE_IMPORT_DESCRIPTOR* idte=(IMAGE_IMPORT_DESCRIPTOR*)dbghelp_api::bi_imagedirectoryentrytodataex((PVOID)modules_helper::get_calling_module((UINT_PTR)p_module_info.module_handle), TRUE, IMAGE_DIRECTORY_ENTRY_IMPORT, &size, &section);
  if(idte == nullptr)
  {
    // This module has no IDT (i.e. it imports nothing).
    return;
  }

  LOG(debug, "Patch IAT module: '{}'", p_module_info.name);

  int64_t thunk_index=-1;
  while(idte->FirstThunk != 0x0)
  {
    ++thunk_index;
    const std::string name=(char*)R2VA(p_module_info.module_handle, idte->Name);

    const std::string imported_module=boost::to_lower_copy(name);
    if(imported_module == p_buginsight_dll_name)
    {
      assert(thunk_index == 0);
      idte++;
      continue;
    }

    if(imported_module == "ucrtbased.dll" || imported_module == "ucrtbase.dll" || imported_module == "vcruntime140.dll" || imported_module == "vcruntime140d.dll")
    {
      p_found_ucrtbase_dll=true;
    }

    // Locate the import's IAT entry.
    IMAGE_THUNK_DATA* thunk=(IMAGE_THUNK_DATA*)R2VA(p_module_info.module_handle, idte->FirstThunk);
    IMAGE_THUNK_DATA* origThunk=(IMAGE_THUNK_DATA*)R2VA(p_module_info.module_handle, idte->OriginalFirstThunk);
    for(; origThunk->u1.Function != NULL; origThunk++, thunk++)
    {
      assert(thunk->u1.Function != origThunk->u1.Function);

      LPVOID func=p_revert_patch?nullptr:iat_helper::find_real_code((LPVOID)thunk->u1.Function);

      auto found_iter=std::find_if(p_functions_to_patch.begin(), p_functions_to_patch.end(), [&](const function_to_patch_t& function_to_patch)
        {
          if(std::find(p_enabled_function_types.begin(), p_enabled_function_types.end(), function_to_patch.m_function_type) == p_enabled_function_types.end())
          {
            return false;
          }

          if(!function_to_patch.m_module_handle)
          {
            return false;
          }

          assert(function_to_patch.m_orginal);
          if(*function_to_patch.m_orginal)
          {
            if(p_revert_patch)
            {
              return (LPVOID)thunk->u1.Function == function_to_patch.m_replacement;
            }

            assert(func);
            return *function_to_patch.m_orginal == func;
          }

          LPVOID import_address=(LPVOID)GetProcAddress(function_to_patch.m_module_handle, function_to_patch.m_function_name.c_str());
          if(!import_address)
          {
            return false;
          }

          import_address=iat_helper::find_real_code(import_address);
          assert(import_address);

          return import_address == func;
        });


      if(found_iter == p_functions_to_patch.end())
      {
        continue;
      }

      const function_to_patch_t& function_to_path=*found_iter;
      iat_helper::patch_function(p_module_info, p_revert_patch, thunk, func, found_iter, function_to_path);
    }

    idte++;
  }
}

void patch_loaded_modules(const std::string& p_buginsight_dll_name, const global_blacklisted_modules_t& p_known_ms_dlls,
  std::vector<function_to_patch_t>& p_functions_to_patch, const user_options_t& p_runtime_options, const bool& p_revert_patch,
  modules_static_runtime_t& p_modules_static_runtime)
{
  std::vector<moduleinfo_t> module_infos;
  modules_helper::get_current_loaded_modules(module_infos);

  for(function_to_patch_t& function : p_functions_to_patch)
  {
    HMODULE module=nullptr;
    const bool module_found=modules_helper::get_module_base_address(module_infos, function.m_module_name, module);
    if(module_found)
    {
      function.set_module_base(module);
    }
  }

  for(const moduleinfo_t& module_info : module_infos)
  {
    if(p_known_ms_dlls.contains(module_info.name))
    {
      continue;
    }

    if(modules_helper::module_has_microsoft_copyright(module_info.path))
    {
      continue;
    }

    const DWORD64 modulebase=(DWORD64)module_info.addrLow;
    if(!modules_helper::get_calling_module((UINT_PTR)modulebase)) // module unloaded
    {
      continue;
    }

    // increase reference count to module
    HMODULE modulelocal=nullptr;
    if(!GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR)modulebase, &modulelocal))
    {
      continue;
    }

    on_scope_exit_t call_on_scope_exit([&modulelocal]()
      {
        const bool ok=FreeLibrary(modulelocal);
        boost::ignore_unused(ok);
        assert(ok);
      });

    static boost::unordered_flat_set<std::wstring> modules_symbols_loaded;
    if(!modules_symbols_loaded.contains(module_info.path.c_str()))
    {
      // Try to load the module's symbols. This ensures that we have loaded the symbols for every module that has ever been loaded into the
      // process, guaranteeing the symbols' availability when generating the leak report.
      IMAGEHLP_MODULEW64  moduleimageinfo{};
      moduleimageinfo.SizeOfStruct=sizeof(IMAGEHLP_MODULE64);
      bool SymbolsLoaded=dbghelp_api::bi_symgetmoduleinfow64(GetCurrentProcess(), modulebase, &moduleimageinfo);

      if(!SymbolsLoaded || moduleimageinfo.BaseOfImage != modulebase)
      {
        DWORD modulesize=(DWORD)(module_info.addrHigh - module_info.addrLow) + 1;

        DWORD64 module= dbghelp_api::bi_symloadmoduleexw(GetCurrentProcess(), nullptr, module_info.path.c_str(), nullptr, modulebase, modulesize, nullptr, 0);
        if(module == modulebase)
        {
          dbghelp_api::bi_symgetmoduleinfow64(GetCurrentProcess(), modulebase, &moduleimageinfo); //ignore errors...
        }
      }

      modules_symbols_loaded.emplace(module_info.path.c_str());
    }

    if(boost::to_lower_copy(module_info.path.string()) == p_buginsight_dll_name)
    {
      continue;
    }

    std::vector<function_type_t> enabled_function_types;

    const auto found_iter_deadlocks=std::find_if(p_runtime_options.m_blacklist_modules_deadlocks.begin(), p_runtime_options.m_blacklist_modules_deadlocks.end(), [&](const blacklist_info_t& p_blacklist_info)
      {
        const std::regex re(p_blacklist_info.m_regex_string);
        std::smatch m;
        const bool match=std::regex_match(module_info.name, m, re);

        return match;
      });

    if(found_iter_deadlocks == p_runtime_options.m_blacklist_modules_deadlocks.end())
    {
      enabled_function_types.push_back(function_type_t::deadlock);
    }

    const auto found_iter_leaks=std::find_if(p_runtime_options.m_blacklist_modules_memory_leaks.begin(), p_runtime_options.m_blacklist_modules_memory_leaks.end(), [&](const blacklist_info_t& p_blacklist_info)
      {
        const std::regex re(p_blacklist_info.m_regex_string);
        std::smatch m;
        const bool match=std::regex_match(module_info.name, m, re);

        return match;
      });

    if(found_iter_leaks == p_runtime_options.m_blacklist_modules_memory_leaks.end())
    {
      enabled_function_types.push_back(function_type_t::memory_leak);
    }

    if(enabled_function_types.empty())
    {
      continue;
    }

    bool found_ucrtbase_dll=false;

    iat_patching::patch_iat(p_buginsight_dll_name, module_info, enabled_function_types, p_functions_to_patch, p_revert_patch, found_ucrtbase_dll);

    if(!found_ucrtbase_dll)
    {
      p_modules_static_runtime.emplace(module_info.name);
    }

  }
}

}
