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

#define SPDLOG_NO_THREAD_ID  // NOLINT(clang-diagnostic-unused-macros)
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_sinks.h>

class my_formater final : public spdlog::formatter
{
public:

  void format(const spdlog::details::log_msg& msg, spdlog::memory_buf_t& dest) override
  {
    auto level_as_string=spdlog::level::to_string_view(msg.level);
    if(level_as_string == "critical")
    {
      level_as_string="fatal";
    }

    fmt::format_to(std::back_inserter(dest), "[BugInsight] [{}] {}\n", level_as_string, msg.payload);
  }

  [[nodiscard]] std::unique_ptr<formatter> clone() const override
  {
    assert(false);
    return {};
  }
};

void init_logger()
{
  const auto stdout_sink=std::make_shared<spdlog::sinks::stdout_sink_mt >();
  stdout_sink->set_formatter(std::make_unique<my_formater>());

  std::vector<spdlog::sink_ptr> sinks{stdout_sink};

  if(IsDebuggerPresent())
  {
    auto sink=std::make_shared<spdlog::sinks::msvc_sink_mt>(false);
    sink->set_formatter(std::make_unique<my_formater>());
    sinks.emplace_back(sink);
  }

  const auto logger=std::make_shared<spdlog::logger>("defaultlogger", sinks.begin(), sinks.end());
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::info);
}
