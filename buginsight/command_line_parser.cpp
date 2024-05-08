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



#include "command_line_parser.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <iterator>

#include <boost/program_options.hpp>

#include "log.hpp"

using namespace boost;
namespace po=boost::program_options;
using namespace std;

namespace command_line_parser
{
template<class T>
ostream& operator<<(ostream& os, const vector<T>& v)
{
  copy(v.begin(), v.end(), ostream_iterator<T>(os, " "));
  return os;
}

bool parse(int ac, wchar_t* av[], configuration_list_t& p_configuration_list, bool& p_start_detached, std::wstring& p_application_full_name_plus_parameter, int& p_exit_code)
{
  try {
    po::options_description generic("Options");
    generic.add_options()
      ("version,v", "print version string")
      ("help,h", "produce help message")
      ;

    po::options_description config("Configuration");
    config.add_options()

      ("on_error", po::value<std::string>(), "fail|fail_on_exit|continue, default: fail_on_exit\n"
        "'fail': Abort immediately with exit code '66'\n"
        "'fail_on_exit': Finish with exit code '66' on process end\n"
        "'continue': application_to_test.exe will end with standard error code\n"
        )

      ("ignore_modules", po::value<std::string>(), "Module list, comma separated:\n"
        "a.dll[{ml}],b.dll[{ml}],c.exe[{ml}],...\n"
        "Module name can contain wildcards: Qt6*.dll\n"
        "Ignore module memory leaks only: a.dll{m}\n"
        "Ignore module dead lock check only: b.dll{l}\n"
        "Disable memory leak check for all modules: *{m}\n"
        "Disable deadlock check for all modules: *{l}\n"
        )

      ("callstacks", po::value<bool>(), "true|false, default: true\n"
        "'true': Call Stacks for locks and memory allocations will be collected.\n"
        "'false': No Call Stacks will be collected. Use for CI tests to improve performance\n"
        "if there are currently no issues open\n")

      ("log_to", po::value<std::string>(), "List of these values, comma separated:\n"
        "stdout|pathtofile.txt\n"
        "default: stdout\n"
        "e.g.: stdout,\"C:/temp/log file.txt\"\n"
        )

      ("log_to_debug_out", po::value<std::string>(), "enabled|disabled|debugger, default: debugger\n"
        "'enabled':  log to debugger output\n"
        "'disabled': no log to debugger output\n"
        "'debugger': log to debugger output if debugger attached\n"
        )

      ("log_level", po::value<std::string>(), "trace|debug|info|warn|error|fatal|off, default: info\n")

      ("exit_code", po::value<int>(), "1..126, exit code on error, default: 66")

      ("start_detached", po::value<bool>(), "true|false, default: false\n"
        "'true': application_to_test.exe will be start non blocking. "
        "BugInsight.exe stops immediately.\n"
        "'false': application_to_test.exe will be start blocking. "
        "BugInsight.exe runs until application_to_test.exe stops.\n"
        )
      ;

    // Hidden options, will be allowed both on command line and
    // in config file, but will not be shown to the user.
    po::options_description hidden("Hidden options");
    hidden.add_options()
      ("application_with_parameter", po::value< vector<std::string> >(), "input file")
      ("thread_watchdog_max_duration_sec", po::value<int>(), "arg: int, default: 30 sec, if a debugger is attached: min 120 sec")
      ("thread_watchdog", po::value<int>(), "arg: false|true, default: true")
      //not yet public ("suppress_by_id", po::value<std::string>(), "suppress_by_id.")
      ;

    po::options_description cmdline_options;
    cmdline_options.add(generic).add(config).add(hidden);

    po::options_description config_file_options;
    config_file_options.add(config).add(hidden);

    po::options_description visible("Usage: BugInsight.exe [Options] [Configuration] -- [\"]application_to_test.exe[\"] [application_to_test options]");
    visible.add(generic).add(config);

    po::positional_options_description p;
    p.add("application_with_parameter", -1);

    po::variables_map vm;
    store(po::wcommand_line_parser(ac, av).options(cmdline_options).positional(p).run(), vm);
    notify(vm);

    if(vm.count("version")) {
      fmt::print("BugInsight version: {}.{}.{}.{}\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, VERSION_REVISION);
      p_exit_code=0;
      return false;
    }

    if(ac == 1 || vm.count("help") || !vm.count("application_with_parameter")) {
      cout << visible << "\n";
      p_exit_code=0;
      return false;
    }

    if(vm.count("callstacks"))
    {
      const auto& value=vm["callstacks"].as<bool>();
      p_configuration_list.emplace_back("callstacks", value?"true":"false");
    }

    if(vm.count("log_to"))
    {
      const auto& value=vm["log_to"].as<std::string>();
      p_configuration_list.emplace_back("log_to", value);
    }

    if(vm.count("log_to_debug_out"))
    {
      const auto& value=vm["log_to_debug_out"].as<std::string>();
      p_configuration_list.emplace_back("log_to_debug_out", value);
    }

    if(vm.count("log_level"))
    {
      const auto& value=vm["log_level"].as<std::string>();
      p_configuration_list.emplace_back("log_level", value);
    }

    if(vm.count("on_error"))
    {
      const auto& value=vm["on_error"].as<std::string>();
      p_configuration_list.emplace_back("on_error", value);
    }

    if(vm.count("start_detached"))
    {
      const auto& value=vm["start_detached"].as<bool>();
      p_configuration_list.emplace_back("start_detached", value?"true":"false");
      p_start_detached=value;
    }

    if(vm.count("thread_watchdog_max_duration_sec"))
    {
      const auto& value=vm["thread_watchdog_max_duration_sec"].as<int>();
      p_configuration_list.emplace_back("thread_watchdog_max_duration_sec", std::to_string(value));
    }

    if(vm.count("thread_watchdog"))
    {
      const auto& value=vm["thread_watchdog"].as<int>();
      p_configuration_list.emplace_back("thread_watchdog", std::to_string(value));
    }

    if(vm.count("exit_code"))
    {
      const auto& value=vm["exit_code"].as<int>();
      p_configuration_list.emplace_back("exit_code", std::to_string(value));
    }

    if(vm.count("suppress_by_id"))
    {
      const auto& value=vm["suppress_by_id"].as<std::string>();
      p_configuration_list.emplace_back("suppress_by_id", value);
    }

    if(vm.count("ignore_modules"))
    {
      const auto& value=vm["ignore_modules"].as<std::string>();
      p_configuration_list.emplace_back("ignore_modules", value);
    }

    if(vm.count("application_with_parameter"))
    {
      const auto& exe_and_parameter=vm["application_with_parameter"].as<vector<string> >();
      assert(!exe_and_parameter.empty());
      for(const auto& i : exe_and_parameter)
      {
        auto as_wstring=std::wstring(i.begin(), i.end());

        p_application_full_name_plus_parameter+=p_application_full_name_plus_parameter.empty()?L"":L" ";
        p_application_full_name_plus_parameter+=as_wstring;
      }

    }
  } catch(std::exception& e)
  {
    LOG(err, "Failed to parse command line parameter: '{}'. Abort.\n", e.what());
    p_exit_code=1;
    return false;
  }

  p_exit_code=0;
  return true;
}
}


