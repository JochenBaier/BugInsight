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

#include "log.hpp"

#define SPDLOG_NO_THREAD_ID
#include <iostream>

#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>
#include "shared_settings.hpp"

class my_formater final : public spdlog::formatter
{
public:
  void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override
  {
    if(msg.level == spdlog::level::info)
    {
      dest.append(msg.payload);
      return;
    }

    auto level_strig=spdlog::level::to_string_view(msg.level);
    fmt::format_to(std::back_inserter(dest), "[BugInsight] [{}] {}\n", level_strig, msg.payload);
  }

  std::unique_ptr<formatter> clone() const override
  {
    return {};
  }
};

void init_logger(const user_options_t& p_runtime_options)
{
  try
  {
    std::vector<spdlog::sink_ptr> sinks;

    if(p_runtime_options.m_log_to_stdout)
    {
      const auto stdout_sink=std::make_shared<spdlog::sinks::stdout_sink_mt >();
      stdout_sink->set_formatter(std::make_unique<my_formater>());
      sinks.emplace_back(stdout_sink);
    }

    if(!p_runtime_options.m_log_to_file.empty())
    {
      const auto file_sink=std::make_shared<spdlog::sinks::basic_file_sink_mt>(p_runtime_options.m_log_to_file, true);
      file_sink->set_formatter(std::make_unique<my_formater>());
      sinks.emplace_back(file_sink);
    }

    if(p_runtime_options.m_log_to_debug_out == log_to_debug_out_options_t::enabled || (p_runtime_options.m_log_to_debug_out == log_to_debug_out_options_t::enabeld_in_debugger && IsDebuggerPresent()))
    {
      const auto sink=std::make_shared<spdlog::sinks::msvc_sink_mt>(false);
      sink->set_formatter(std::make_unique<my_formater>());
      sinks.emplace_back(sink);
    }

    const auto logger=std::make_shared<spdlog::logger>("defaultlogger", sinks.begin(), sinks.end());

    spdlog::set_default_logger(logger);
    spdlog::set_level(p_runtime_options.m_log_level);
  } catch(std::exception& p_ex)
  {
    char error_buffer[1024]{};

    sprintf_s(error_buffer, "[%s] [fatal] Failed to initialize Logger: '%s'. Abort.\n", APP_NAME_PRETTY, p_ex.what());
    std::cout << error_buffer;
    fflush(stdout);

    OutputDebugStringA(error_buffer);

    TerminateProcess(GetCurrentProcess(), 1);
  }
}
