// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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
