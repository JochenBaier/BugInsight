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

#pragma  once

#define SPDLOG_NO_THREAD_ID
#define SPDLOG_WCHAR_TO_UTF8_SUPPORT

#include "spdlog/spdlog.h"

using namespace spdlog;  // NOLINT(clang-diagnostic-header-hygiene)

#define LOG(l, ...) if (spdlog::should_log(level::l)) spdlog::log(level::l, __VA_ARGS__); static_assert(true, "")

struct user_options_t;

void init_logger(const user_options_t& p_runtime_options);
