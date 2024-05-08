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

#include <Windows.h>
#include <vector>
#include <string>

#include <boost/unordered/unordered_flat_set.hpp>

#include "modules_helper.hpp"

struct function_to_patch_t;

using global_blacklisted_modules_t=boost::unordered_flat_set<std::string>;

namespace iat_helper
{
void patch_function(const moduleinfo_t& p_module_info, const bool& p_revert_patch, IMAGE_THUNK_DATA* p_thunk, LPVOID p_func, const std::vector<function_to_patch_t>::iterator& p_found_iter,
  const function_to_patch_t& p_function_to_path);
LPVOID find_real_code(LPVOID pCode);
}
