#pragma once

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

