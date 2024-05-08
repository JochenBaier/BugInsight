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

#include <fmt/std.h>
#include "iat_helper.hpp"

#include "iat_patching.hpp"

typedef FARPROC(__stdcall* GetProcAddress_t) (HMODULE, LPCSTR);

class loader_t
{
public:
  loader_t();
  ~loader_t();
  void patch_loaded_modules();
  void unpatch_loaded_modules();
  void* getprocaddress_patched(HMODULE module, LPCSTR procname);
  bool is_global_blacklisted_module(const HMODULE& p_module) const;

  std::atomic_bool m_init_finished{false};

private:
  void init_known_microsoft_modules_list();
  global_blacklisted_modules_t m_global_blacklisted_modules;
  std::vector<function_to_patch_t> m_functions_to_patch;
  std::string m_exe_module_name;
  boost::filesystem::path m_file_path_of_buginsight_module;
  modules_static_runtime_t m_modules_static_runtime;
  HANDLE m_current_process{GetCurrentProcess()};
};
