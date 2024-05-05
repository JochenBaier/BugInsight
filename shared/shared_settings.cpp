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


#include "shared_settings.hpp"

#include <fmt/xchar.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/interprocess/managed_windows_shared_memory.hpp>

namespace shared_settings
{
std::string user_options_to_string(const user_options_t& p_user_options)
{
  std::stringstream ss;
  boost::archive::text_oarchive oa{ss};

  oa << p_user_options;

  std::string as_string=ss.str();
  assert(!as_string.empty());
  return as_string;
};

void user_options_from_string(const std::string& p_options_string, user_options_t& p_user_options)
{
  std::stringstream aa;
  aa << p_options_string;

  boost::archive::text_iarchive ia{aa};
  ia >> p_user_options;
}

void save_user_options_to_shared_memory(boost::interprocess::managed_windows_shared_memory& p_managed_shm, const uint32_t& p_process_id, const user_options_t& p_user_options)
{
  const std::string shared_memory_name=fmt::format("de/{}/shared_memory/{}", boost::to_lower_copy(std::string(APP_NAME_PRETTY)), p_process_id);
  std::string user_options_string=shared_settings::user_options_to_string(p_user_options);
  p_managed_shm=boost::interprocess::managed_windows_shared_memory{boost::interprocess::open_or_create, shared_memory_name.c_str(), 1024 + user_options_string.size() + 1};

  char* i=p_managed_shm.construct<char>("user_options")[user_options_string.size() + 1](0);
  memcpy_s(i, p_managed_shm.get_size() - 1 - 1024, user_options_string.c_str(), user_options_string.size());
}

void get_user_options_from_shared_memory(user_options_t& p_user_options)
{
  const std::string shared_memory_name=fmt::format("de/{}/shared_memory/{}", boost::to_lower_copy(std::string(APP_NAME_PRETTY)), GetCurrentProcessId());
  boost::interprocess::managed_windows_shared_memory segment(boost::interprocess::open_only, shared_memory_name.c_str());

  const std::pair<char*, std::size_t> p=segment.find<char>("user_options");
  assert(p.first);
  assert(p.second);

  const std::string as_string=p.first;
  user_options_from_string(as_string, p_user_options);
}
}
