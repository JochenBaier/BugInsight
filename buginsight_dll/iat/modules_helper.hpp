#pragma once

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

#include <Windows.h>

#include <boost/filesystem.hpp>
#include <boost/unordered/unordered_flat_set.hpp>

// This structure stores information, primarily the virtual address range, about
// a given module and can be used with the Set template because it supports the
// '<' operator (sorts by virtual address range).
struct moduleinfo_t {
  BOOL operator < (const struct moduleinfo_t& other) const
  {
    if(addrHigh < other.addrLow)
    {
      return TRUE;
    }

    return FALSE;
  }

  HMODULE module_handle{};
  SIZE_T addrLow{};                  // Lowest address within the module's virtual address space (i.e. base address).
  SIZE_T addrHigh{};                 // Highest address within the module's virtual address space (i.e. base + size).
  UINT32 flags{};                    // Module flags:
  std::string name;                  // The module's name (e.g. "kernel32.dll").
  boost::filesystem::path path;      // The fully qualified path from where the module was loaded.
};

using modules_static_runtime_t=boost::unordered_flat_set<std::string>;

namespace modules_helper
{
boost::filesystem::path name_of_current_module();
bool module_has_microsoft_copyright(const boost::filesystem::path& p_path);
void get_current_loaded_modules(std::vector<moduleinfo_t>& p_module_infos);
bool get_module_base_address(const std::vector<moduleinfo_t>& p_module_infos, const std::string& p_module_name, HMODULE& p_module);
HMODULE get_calling_module(UINT_PTR pCaller);
}

