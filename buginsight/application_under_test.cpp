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


#include "application_under_test.hpp"

#include <Windows.h>
#include <detours/detours.h>

#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/interprocess/managed_windows_shared_memory.hpp>

#include "log.hpp"
#include "scope_exit.hpp"
#include "shared_settings.hpp"

HANDLE g_detached_event{};
boost::interprocess::managed_windows_shared_memory g_shared_memory_segment;
extern user_options_t g_user_options;

BOOL detour_create_process_routinew(LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes,
  BOOL bInheritHandles, DWORD dwCreationFlags, LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation);

int start_application_under_test(const bool& start_detached, const std::wstring& application_full_name_plus_parameter, const boost::filesystem::path& dll)
{
  HANDLE stdout_pipe[2]{};

  PROCESS_INFORMATION pi{};
  STARTUPINFOW si{};
  si.cb=sizeof(si);

  if(!start_detached)
  {
    bool ok=CreatePipe(stdout_pipe + 0, stdout_pipe + 1, nullptr, 0);
    if(!ok)
    {
      const DWORD error=::GetLastError();
      const std::string error_message=std::system_category().message((int)error);
      const std::wstring werror_message=std::wstring(error_message.begin(), error_message.end());
      LOG(critical, L"Failed to call 'CreatePipe(stdout_pipe)': {}", werror_message);
      return 1;
    }

    ok=SetHandleInformation(stdout_pipe[1], HANDLE_FLAG_INHERIT, HANDLE_FLAG_INHERIT);
    assert(ok);

    si.hStdOutput=stdout_pipe[1];
    si.hStdError=stdout_pipe[1];
    si.dwFlags|=STARTF_USESTDHANDLES;
  }

  DWORD  dwCreationFlags=CREATE_DEFAULT_ERROR_MODE | CREATE_SUSPENDED;
  if(start_detached)
  {
    dwCreationFlags|=DETACHED_PROCESS;
  }

  const bool detour_call_ok=DetourCreateProcessWithDllExW(nullptr,
    const_cast<LPWSTR>(application_full_name_plus_parameter.c_str()),
    nullptr,
    nullptr,
    !start_detached,
    dwCreationFlags,
    nullptr, nullptr, &si, &pi,
    dll.string().c_str(), detour_create_process_routinew);

  if(!detour_call_ok)
  {
    const DWORD error=::GetLastError();
    const std::string error_message=std::system_category().message((int)error);
    const std::wstring werror_message=std::wstring(error_message.begin(), error_message.end());
    LOG(err, L"Failed to execute application '{}': '{}'. Abort.\n", application_full_name_plus_parameter, werror_message);
    return 1;
  }

  on_scope_exit_t cleanup([&]
    {
      CloseHandle(pi.hProcess);
      CloseHandle(pi.hThread);
    });

  const DWORD resume_res=ResumeThread(pi.hThread);
  if(resume_res == (DWORD)-1)
  {
    const DWORD error=::GetLastError();
    const std::string error_message=std::system_category().message((int)error);
    const std::wstring werror_message=std::wstring(error_message.begin(), error_message.end());
    LOG(err, L"Failed to resume started application '{}': '{}'. Abort.\n", application_full_name_plus_parameter, werror_message);
    return 1;
  }

  if(start_detached)
  {
    DWORD res=WaitForSingleObject(g_detached_event, 60000);
    if(res != WAIT_OBJECT_0)
    {
      std::wstring werror_message=L"Timeout";
      if(res == WAIT_FAILED)
      {
        const DWORD error=::GetLastError();
        const std::string error_message=std::system_category().message((int)error);
        werror_message=std::wstring(error_message.begin(), error_message.end());
      }

      LOG(err, L"Failed to get responce from detached application '{}' within 60000 seconds: '{}'. Abort.\n", application_full_name_plus_parameter, werror_message);
    }

    CloseHandle(g_detached_event);
    return 0;
  }

  const bool ok=CloseHandle(stdout_pipe[1]);
  boost::ignore_unused(ok);
  assert(ok);

  DWORD dwRead, dwWritten;
  CHAR chBuf[4096];

  // Read from the pipe that is the standard output for child process.
  for(;;)
  {
    const DWORD ret=WaitForSingleObject(pi.hProcess, 100);

    // Check if data is available to read from the pipe
    DWORD dwAvail{};
    if(!PeekNamedPipe(stdout_pipe[0], nullptr, 0, nullptr, &dwAvail, nullptr))
    {
      const DWORD error=::GetLastError();
      const std::string error_message=std::system_category().message((int)error);
      const std::wstring werror_message=std::wstring(error_message.begin(), error_message.end());
      LOG(warn, L"Call PeekNamedPipe() failed: '{}'.\n", werror_message);
    }

    if(dwAvail > 0)
    {
      // Read the data from the pipe
      if(!ReadFile(stdout_pipe[0], chBuf, sizeof(chBuf), &dwRead, nullptr) || dwRead == 0)
      {
        break;
      }

      // Print the read data to the parent console
      if(!WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), chBuf, dwRead, &dwWritten, nullptr))
      {
        break;
      }
    }

    if(ret != WAIT_TIMEOUT)
    {
      break;
    }

  }

  DWORD dwResult=0;
  if(!GetExitCodeProcess(pi.hProcess, &dwResult))
  {
    const DWORD error=::GetLastError();
    const std::string error_message=std::system_category().message((int)error);
    const std::wstring werror_message=std::wstring(error_message.begin(), error_message.end());
    LOG(err, L"Failed to get exit code from finished application '{}': '{}'. Abort.\n", application_full_name_plus_parameter, werror_message);
    return 1;
  }

  LOG(info, L"Application under test finished with exit code '{}'\n\n", (int)dwResult);
  return (int)dwResult;
}

int init_start_detached(const uint32_t& p_process_id)
{
  const std::string event_name=fmt::format("de/{}/start_detached_event/{}", boost::to_lower_copy(std::string(APP_NAME_PRETTY)), p_process_id);
  g_detached_event=CreateEvent(nullptr, FALSE, FALSE, event_name.c_str());
  if(!g_detached_event)
  {
    const std::string error_message=std::system_category().message((int)GetLastError());
    LOG(critical, "CreateEvent({}) call failed: {}.Abort.\n", event_name, error_message);
    return 1;
  }
  return 0;
}

BOOL detour_create_process_routinew(
  LPCWSTR lpApplicationName, LPWSTR lpCommandLine, LPSECURITY_ATTRIBUTES lpProcessAttributes, LPSECURITY_ATTRIBUTES lpThreadAttributes, BOOL bInheritHandles, DWORD dwCreationFlags,
  LPVOID lpEnvironment, LPCWSTR lpCurrentDirectory, LPSTARTUPINFOW lpStartupInfo, LPPROCESS_INFORMATION lpProcessInformation)
{
  const bool ok=CreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
    dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
  if(!ok)
  {
    return false;
  }

  shared_settings::save_user_options_to_shared_memory(g_shared_memory_segment, lpProcessInformation->dwProcessId, g_user_options);

  if(g_user_options.m_start_detached)
  {
    const int res=init_start_detached(lpProcessInformation->dwProcessId);
    if(res != 0)
    {
      return false;
    }
  }

  return true;
}


