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

#include <cassert>
#include <Windows.h>
#include <string>
#include <vector>
#include <boost/algorithm/string/case_conv.hpp>

#include "modules_helper.hpp"

enum class function_type_t
{
  memory_leak,
  deadlock,
  other
};

struct function_to_patch_t
{
  // ReSharper disable once CppPassValueParameterByConstReference
  // ReSharper disable once CppPassValueParameterByConstReference
  function_to_patch_t(const function_type_t p_function_type, std::string p_function_name, std::string p_module, LPVOID* p_orginal, void* p_replacement) :
    m_function_type(p_function_type),
    m_function_name(p_function_name),
    m_module_name(p_module),
    m_orginal(p_orginal),
    m_replacement(p_replacement)
  {
    assert(m_replacement);
    assert(m_module_name == boost::to_lower_copy(m_module_name));
  }

  void set_module_base(const HMODULE& p_module_base)
  {
    m_module_handle=p_module_base;
  }

  function_type_t m_function_type;
  std::string m_function_name;
  std::string m_module_name;
  HMODULE  m_module_handle{};

  LPVOID* m_orginal;
  LPVOID m_replacement;
};

namespace patched_functions
{
void prepare_functions_to_patch(std::vector<function_to_patch_t>& m_functions_to_patch);
}
