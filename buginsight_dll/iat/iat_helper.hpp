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
