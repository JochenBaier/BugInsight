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

#include "modules_helper.hpp"

#include <cassert>
#include <strsafe.h>
#include <vector>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <boost/unordered/unordered_flat_set.hpp>

#include "loader.hpp"
#include "fmt/printf.h"
#include "log.hpp"
#include "dbghelp_api.hpp"


namespace modules_helper
{
boost::filesystem::path name_of_current_module()
{
  char path[2048]{};
  HMODULE hm=nullptr;

  if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&name_of_current_module, &hm) == 0)
  {
    assert(false);
  }

  if(GetModuleFileName(hm, path, sizeof(path)) == 0)
  {
    assert(false);
  }

  return {path};
}

bool module_has_microsoft_copyright(const boost::filesystem::path& p_path)
{
  // Retrieve the version information size
  const DWORD versionInfoSize=GetFileVersionInfoSizeW(p_path.c_str(), nullptr);
  if(!versionInfoSize)
  {
    return false;
  }

  std::vector<uint8_t> versionInfoBuffer(versionInfoSize);
  if(!GetFileVersionInfoW(p_path.c_str(), 0, versionInfoSize, versionInfoBuffer.data()))
  {
    return false;
  }

  struct LANGANDCODEPAGE
  {
    WORD wLanguage;
    WORD wCodePage;
  } *lpTranslate;

  UINT cbTranslate;
  if(!VerQueryValueA(versionInfoBuffer.data(), "\\VarFileInfo\\Translation", (LPVOID*)&lpTranslate, &cbTranslate))
  {
    return false;
  }

  // Read the file description for each language and code page.
  for(size_t i=0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++)
  {
    const std::string sub_block=fmt::sprintf(R"(\StringFileInfo\%04x%04x\CompanyName)", lpTranslate[i].wLanguage, lpTranslate[i].wCodePage);

    void* companyname{};
    UINT companynameSize{};

    if(!VerQueryValueA(versionInfoBuffer.data(), sub_block.c_str(), &companyname, &companynameSize))
    {
      return false;
    }

    if(!companynameSize)
    {
      return false;
    }

    const std::string company_name_std_string=(char*)companyname;

    assert(companyname);
    if(boost::contains(boost::to_lower_copy(company_name_std_string), "microsoft"))
    {
      //wprintf(L"%s\n", p_path.filename().c_str());
      return true;
    }
  }

  return false;
}

BOOL add_loaded_module_callback(PCWSTR modulepath, DWORD64 modulebase, ULONG modulesize, PVOID context)
{
  assert(modulepath);
  assert(context);

  const boost::filesystem::path modulepathw(modulepath);

  std::vector<moduleinfo_t>* module_infos=(std::vector<moduleinfo_t>*) context;

  moduleinfo_t  moduleinfo;
  moduleinfo.module_handle=(HMODULE)modulebase;
  moduleinfo.addrLow=(UINT_PTR)modulebase;
  moduleinfo.addrHigh=(UINT_PTR)(modulebase + modulesize) - 1;
  moduleinfo.flags=0x0;
  moduleinfo.path=modulepathw;
  moduleinfo.name=boost::to_lower_copy(modulepathw.filename().string());

  LOG(trace, "EnumerateLoadedModulesW64 callback: '{}'", moduleinfo.name.c_str());

  module_infos->push_back(moduleinfo);
  return TRUE;
}

BOOL enumerateloadedmodulesw64(__in HANDLE hProcess, __in PENUMLOADED_MODULES_CALLBACKW64 EnumLoadedModulesCallback, __in_opt PVOID UserContext)
{
  return dbghelp_api::bi_enumerateloadedmodulesw64(hProcess, EnumLoadedModulesCallback, UserContext);
}

void get_current_loaded_modules(std::vector<moduleinfo_t>& p_module_infos)
{
  modules_helper::enumerateloadedmodulesw64(GetCurrentProcess(), add_loaded_module_callback, &p_module_infos);
  assert(!p_module_infos.empty());
}

bool get_module_base_address(const std::vector<moduleinfo_t>& p_module_infos, const std::string& p_module_name, HMODULE& p_module)
{
  const auto& found_iter=std::find_if(p_module_infos.begin(), p_module_infos.end(), [&](const moduleinfo_t& moduleinfo)
    {
      return moduleinfo.name == p_module_name;
    });

  if(found_iter == p_module_infos.end())
  {
    return false;
  }

  p_module=found_iter->module_handle;
  return true;
}

HMODULE get_calling_module(UINT_PTR pCaller)
{
  HMODULE hModule=nullptr;
  MEMORY_BASIC_INFORMATION mbi;
  if(VirtualQuery((LPCVOID)pCaller, &mbi, sizeof(MEMORY_BASIC_INFORMATION)) == sizeof(MEMORY_BASIC_INFORMATION))
  {
    // the allocation base is the beginning of a PE file
    hModule=(HMODULE)mbi.AllocationBase;
  }
  return hModule;
}

}
