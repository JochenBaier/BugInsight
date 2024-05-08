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
