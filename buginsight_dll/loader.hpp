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
