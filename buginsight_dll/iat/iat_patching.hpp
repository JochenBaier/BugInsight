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

#include <vector>
#include <string>

#include "iat_helper.hpp"
#include "modules_helper.hpp"

struct user_options_t;

namespace iat_patching
{
void patch_loaded_modules(const std::string& p_buginsight_dll_name, const global_blacklisted_modules_t& p_known_ms_dlls, std::vector<function_to_patch_t>& p_functions_to_patch,
  const user_options_t& p_runtime_options, const bool& p_revert_patch, modules_static_runtime_t& p_modules_static_runtime);
}