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
