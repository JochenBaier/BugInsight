// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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

//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#include "memory_helper.hpp" 

#include "log_helper.hpp"
#include "memory_leak_map_with_callstack.hpp"

extern user_options_t g_user_options;

namespace memory_helper
{
TM_ATTRIBUTE_NOINLINE bool log_or_abort_if_memory_adddress_is_unknown(memory_leak_repository_t& p_memory_leak_repository, void* _Block, api_family_t p_api_family, api_functions_t p_api_function)
{
  api_family_t api_family=api_family_t::e_unknow;
  boost::span<uint8_t> user_memory;
  api_functions_t api_function_stored=api_functions_t::e_unknown;
  const bool found=p_memory_leak_repository.get_data((uintptr_t)_Block, api_family, user_memory, api_function_stored);
  if(!found)
  {
    auto log_text=fmt::memory_buffer();
    log_helper::build_unknown_memory_address_text(std::this_thread::get_id(), p_api_function, _Block, log_text);
    log_helper::process_error(log_text, g_user_options, false);
  } else
  {
    if(api_family != p_api_family)
    {
      auto log_text=fmt::memory_buffer();
      log_helper::build_wrong_api_family_text(std::this_thread::get_id(), p_api_function, api_family, p_api_family, _Block, log_text);
      log_helper::process_error(log_text, g_user_options, false);
    }
  }

  return found;
}

}
