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


#include <cstdio>
#include <Windows.h>

#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <boost/dll.hpp>

#include "log.hpp"
#include "command_line_parser.hpp"
#include "application_under_test.hpp"
#include "options_parser.hpp"
#include "shared_settings.hpp"

BOOL CtrlHandler(DWORD fdwCtrlType);
int pre_checks(boost::filesystem::path& dll);

user_options_t g_user_options;

int wmain(int ac, wchar_t* av[])
{
  try
  {
    const bool set_ok=SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
    assert(set_ok);
    boost::ignore_unused(set_ok);

    init_logger();

    boost::filesystem::path dll;
    int exit_code=pre_checks(dll);
    if(exit_code)
    {
      return exit_code;
    }

    configuration_list_t configuration_list;
    bool start_detached=false;
    std::wstring application_full_name_plus_parameter;
    const bool ok=command_line_parser::parse(ac, av, configuration_list, start_detached, application_full_name_plus_parameter, exit_code);
    if(!ok)
    {
      return exit_code;
    }

    if(!parse_options(configuration_list, g_user_options))
    {
      return 1;
    }

    spdlog::set_level(g_user_options.m_log_level);

    LOG(info, L"{} Application under test: '{}'", std::wstring(APP_WNAME_PRETTY), application_full_name_plus_parameter);

    exit_code=start_application_under_test(start_detached, application_full_name_plus_parameter, dll);
    return exit_code;
  } catch(std::exception& p_ex)
  {
    LOG(critical, "Exception caught in main: '{}'", p_ex.what());
    return 1;
  }
}

int pre_checks(boost::filesystem::path& dll)
{
  const boost::filesystem::path exe_full_file_name=boost::dll::program_location();
  boost::filesystem::path exe_folder=exe_full_file_name.parent_path();

#ifdef _WIN64

#ifdef _DEBUG
  const std::string buginsight_dll_name="BugInsightDllD64.dll";
#else
  const std::string buginsight_dll_name="BugInsightDll64.dll";
#endif

#else
#ifdef _DEBUG
  const std::string buginsight_dll_name="BugInsightDllD32.dll";
#else
  const std::string buginsight_dll_name="BugInsightDll32.dll";
#endif

#endif

  dll=exe_folder.append(buginsight_dll_name);
  if(!boost::filesystem::exists(dll))
  {
    LOG(critical, "Needed DLL '{}' not found in folder '{}'.\nPlease reinstall. Abort.\n", buginsight_dll_name, exe_full_file_name.parent_path().string());
    return 1;
  }

  return 0;
}

BOOL CtrlHandler(DWORD fdwCtrlType)
{
  switch(fdwCtrlType)
  {
  case CTRL_C_EVENT:
  {
    return TRUE;
  }
  default:
  {
    return FALSE;
  }
  }
}

