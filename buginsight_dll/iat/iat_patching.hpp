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