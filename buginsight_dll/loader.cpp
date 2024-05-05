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

//Based on VLD, Copyright © 2005-2017 VLD Team
//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#include "loader.hpp"

#include <psapi.h>

#include <boost/dll.hpp>
#include <boost/algorithm/string.hpp>

#include "getprocaddress_monitor.hpp"
#include "deadlock_monitor.hpp"
#include "ldr_loader_lock.hpp"
#include "log.hpp"
#include "log_helper.hpp"
#include "memory_leak_map_with_callstack.hpp"
#include "run_time_options.hpp"
#include "stacktrace.h"
#include "patched_functions.hpp"
#include "shared_settings.hpp"
#include "dbghelp_api.hpp"

user_options_t g_user_options;
statistics_t g_statistics;
deadlock_monitor_t* g_deadlock_monitor=nullptr;
memory_leak_repository_t* g_memory_leak_repository=nullptr;
LdrLockLoaderLock_t LdrLockLoaderLock=nullptr;
LdrUnlockLoaderLock_t LdrUnlockLoaderLock=nullptr;
std::atomic_uint64_t g_monitored_api_calls{};
HANDLE detached_event{};

loader_t::loader_t()
{
  shared_settings::get_user_options_from_shared_memory(g_user_options);

  init_logger(g_user_options);
  spdlog::flush_on(level::trace);

  const boost::filesystem::path exe_full_file_name=boost::dll::program_location();
  m_exe_module_name=boost::filesystem::path(exe_full_file_name).filename().string();
  m_file_path_of_buginsight_module=modules_helper::name_of_current_module();

  LOG(trace, "%s [info] Loaded '%s' into application unter test.", std::string(APP_NAME_PRETTY).c_str());

  const HMODULE ntdll=GetModuleHandleW(L"ntdll.dll");
  assert(ntdll);
  LdrLockLoaderLock=(LdrLockLoaderLock_t)GetProcAddress(ntdll, "LdrLockLoaderLock");
  assert(LdrLockLoaderLock);
  LdrUnlockLoaderLock=(LdrUnlockLoaderLock_t)GetProcAddress(ntdll, "LdrUnlockLoaderLock");
  assert(LdrUnlockLoaderLock);

  //not yet used dbghelp_api::loaddll();

  if(g_user_options.m_start_detached)
  {
    const std::string event_name=fmt::format("de/{}/start_detached_event/{}", boost::to_lower_copy(std::string(APP_NAME_PRETTY)), GetCurrentProcessId());
    detached_event=CreateEvent(nullptr, FALSE, FALSE, event_name.c_str());
    if(!detached_event)
    {
      const std::string error_message=std::system_category().message((int)GetLastError());
      LOG(critical, "CreateEvent({}) call failed: {}. Abort.\n", event_name, error_message);
      TerminateProcess(GetCurrentProcess(), 1);
    }

    const bool set_ok=SetEvent(detached_event);
    if(!set_ok)
    {
      const std::string error_message=std::system_category().message((int)GetLastError());
      LOG(critical, "SetEvent({}) call failed: '{}'. Abort.\n", event_name, error_message);
      TerminateProcess(GetCurrentProcess(), 1);
    }

    CloseHandle(detached_event);
  }

  init_known_microsoft_modules_list();

  InitializeSymbolizer();

  {//scope
    ldr_loader_lock_t ll;

    patched_functions::prepare_functions_to_patch(m_functions_to_patch);

    g_deadlock_monitor=new deadlock_monitor_t(g_user_options);
    g_memory_leak_repository=new memory_leak_repository_t(g_user_options.m_collect_callstacks);

    patch_loaded_modules();

    LOG(info, "[BugInsight] [info] BugInsight loaded (version: '{}.{}.{}.{}')\n", VERSION_MAJOR, VERSION_MINOR, VERSION_BUILD, VERSION_REVISION);

    auto out=fmt::memory_buffer();
    log_helper::create_options_log_text(g_user_options, out);
    LOG(info, fmt::to_string(out));

    m_init_finished=true;
  }

  spdlog::flush_on(level::warn);
}

void loader_t::patch_loaded_modules()
{
  assert(!m_functions_to_patch.empty());
  iat_patching::patch_loaded_modules(boost::to_lower_copy(m_file_path_of_buginsight_module.string()), m_global_blacklisted_modules,
    m_functions_to_patch, g_user_options, false, m_modules_static_runtime);
}

void loader_t::unpatch_loaded_modules()
{
  assert(!m_functions_to_patch.empty());
  modules_static_runtime_t modules_static_runtime_unused;

  iat_patching::patch_loaded_modules(boost::to_lower_copy(m_file_path_of_buginsight_module.string()), m_global_blacklisted_modules, m_functions_to_patch, g_user_options,
    true, modules_static_runtime_unused);
}

//not yet public 
void* loader_t::getprocaddress_patched(HMODULE module, LPCSTR procname)
{
  const FARPROC orignal=GetProcAddress_fptr(module, procname);
  if(!orignal)
  {
    return (void*)orignal;
  }

  auto found_iter=std::find_if(m_functions_to_patch.begin(), m_functions_to_patch.end(), [&](const function_to_patch_t& function_to_patch)
    {
      const bool match=function_to_patch.m_module_handle == module && strcmp(function_to_patch.m_function_name.c_str(), procname) == 0 /*&& *function_to_patch.m_orginal == (void*)orignal*/ && function_to_patch.m_replacement;
      return match;
    });

  if(found_iter == m_functions_to_patch.end())
  {
    return (void*)orignal;
  }

  assert(found_iter->m_function_name == procname);
  return found_iter->m_replacement;
}

bool loader_t::is_global_blacklisted_module(const HMODULE& p_module) const
{
  wchar_t modulePath[2048]{};
  if(!GetModuleFileNameExW(m_current_process, p_module, modulePath, sizeof(modulePath) / sizeof(wchar_t)))
  {
    return true;
  }

  const boost::filesystem::path module_path(modulePath);

  const bool is=m_global_blacklisted_modules.contains(boost::to_lower_copy(module_path.filename().string()));
  return is;
}

loader_t::~loader_t()
{
  m_init_finished=false;

  LOG(info, "The application under test is currently being shut down...\n");

  ldr_loader_lock_t ll;

  unpatch_loaded_modules();

  assert(g_deadlock_monitor);
  assert(g_memory_leak_repository);

  const bool potential_deadlocks_found=g_deadlock_monitor->potential_deadlocks_found();
  const bool other_errros_found=g_deadlock_monitor->other_errors_found();
  delete g_deadlock_monitor;


  std::vector<used_api_info_t> used_api_infos;
  used_api_infos.resize((size_t)api_functions_t::e_SIZE);
  for(size_t i=0; i < used_api_infos.size(); ++i)
  {
    used_api_infos[i].m_api_function=(api_functions_t)i;
  }

  const bool memory_leaks_found=g_memory_leak_repository->leak_count() > 0;
  bool leaks_suppressed=false;

  if(memory_leaks_found)
  {
    LOG(info, "Analysing '{}' leaks...\n", g_memory_leak_repository->leak_count());

    const bool found=g_memory_leak_repository->group_leaks2(used_api_infos);
    boost::ignore_unused(found);
    assert(found);

    auto memory_leak_report=fmt::memory_buffer();
    memory_leak_repository_t::generate_leak_report2(memory_leak_report, used_api_infos);

    if(g_user_options.m_collect_callstacks)
    {
      memory_leak_repository_t::generate_callstacks2(memory_leak_report, used_api_infos, g_user_options.m_ids_to_suppress, leaks_suppressed, m_modules_static_runtime);
      LOG(info, fmt::to_string(memory_leak_report));
    } else
    {
      LOG(warn, "Memory leaks found but collecting Call Stacks is disabled! Use command line option '--callstacks=true' to enable.");
    }
  }

  auto summary=fmt::memory_buffer();
  log_helper::create_summary(g_statistics, potential_deadlocks_found, other_errros_found, memory_leaks_found, leaks_suppressed, g_monitored_api_calls.load(), summary);
  LOG(info, fmt::to_string(summary));

  const bool abort=potential_deadlocks_found || other_errros_found || memory_leaks_found || g_monitored_api_calls == 0;
  if(abort && g_user_options.m_on_error != on_error_option_t::_continue)
  {
    LOG(err, "Terminate process with exit code '{}' due to errors. Use command line option '--on_error=continue' to ignore errors.", g_user_options.m_exit_code);
    spdlog::shutdown();
    TerminateProcess(GetCurrentProcess(), g_user_options.m_exit_code);
    return;
  }
  spdlog::shutdown();

}
void loader_t::init_known_microsoft_modules_list()
{
  m_global_blacklisted_modules=
  {
    "apphelp.dll",
    "kernel32.dll",
    "kernelbase.dll",
    "msvcp140.dll",
    "msvcp140d.dll",
    "oleaut32.dll",
    "rpcrt4.dll",
    "vcruntime140.dll",
    "vcruntime140d.dll",
    "vcruntime140_1d.dll",
    "version.dll",
    "bcryptprimitives.dll",
    "combase.dll",
    "dbghelp.dll",
    "kernel.appcore.dll",
    "msvcp_win.dll",
    "msvcrt.dll",
    "ntdll.dll",
    "ucrtbase.dll",
    "ucrtbased.dll",
    "ws2_32.dll",
    "advapi32.dll",
    "sechost.dll",
    "crypt32.dll",
    "cryptbase.dll",
    "cryptsp.dll",
    "gdi32.dll",
    "imm32.dll",
    "user32.dll",
    "bcrypt.dll",
    "gdi32full.dll",
    "rsaenh.dll",
    "win32u.dll",
    "textshaping.dll",
    "uxtheme.dll",
    "msctf.dll",
    "textinputframework.dll",
    "shell32.dll",
    "coremessaging.dll",
    "coreuicomponents.dll",
    "dnsapi.dll",
    "iphlpapi.dll",
    "msasn1.dll",
    "nlaapi.dll",
    "nsi.dll",
    "shcore.dll",
    "winhttp.dll",
    "winmm.dll",
    "wintrust.dll",
    "wldp.dll",
    "mswsock.dll",
    "napinsp.dll",
    "ntmarta.dll",
    "ole32.dll",
    "pnrpnsp.dll",
    "shlwapi.dll",
    "windows.storage.dll",
    "winrnr.dll",
    "wintypes.dll",
    "wshbth.dll",
    "vcruntime140_1.dll",
    "authz.dll",
    "mpr.dll",
    "msvcp140_1d.dll",
    "netapi32.dll",
    "netutils.dll",
    "shell32.dll",
    "srvcli.dll",
    "userenv.dll",
    "dwmapi.dll",
    "fltlib.dll",
    "mscoree.dll",
    "msvcp140_clr0400.dll",
    "microsoft.visualstudio.designtools.wpftapbootstrap.dll",
    "propsys.dll",
    "psapi.dll",
    "presentationcore.ni.dll",
    "presentationframework.aero2.ni.dll",
    "presentationframework.ni.dll",
    "presentationnative_v0400.dll",
    "smdiagnostics.ni.dll",
    "spsys.dll",
    "sspicli.dll",
    "system.configuration.ni.dll",
    "system.core.ni.dll",
    "system.drawing.ni.dll",
    "system.runtime.serialization.ni.dll",
    "system.servicemodel.internals.ni.dll",
    "system.windows.forms.ni.dll",
    "system.xaml.ni.dll",
    "system.xml.ni.dll",
    "system.ni.dll",
    "uiautomationcore.dll",
    "uiautomationprovider.ni.dll",
    "uiautomationtypes.ni.dll",
    "vcruntime140_clr0400.dll",
    "winsta.dll",
    "windowsbase.ni.dll",
    "windowscodecs.dll",
    "clbcatq.dll",
    "clr.dll",
    "clrjit.dll",
    "d3d11.dll",
    "d3d9.dll",
    "dataexchange.dll",
    "dcomp.dll",
    "dwrite.dll",
    "dxgi.dll",
    "iertutil.dll",
    "mscoreei.dll",
    "mscorlib.ni.dll",
    "msctfui.dll",
    "powrprof.dll",
    "profapi.dll",
    "twinapi.appcore.dll",
    "ucrtbase_clr0400.dll",
    "urlmon.dll",
    "virtdisk.dll",
    "wpfgfx_v0400.dll",
    "wtsapi32.dll",
    "Microsoft.VisualStudio.Debugger.Runtime.Impl.dll",
    "dpapi.dll",
    "odbc32.dll",
    "secur32.dll",
    "rasadhlp.dll",
    "fwpuclnt.dll",
    "vcruntime140_1_clr0400.dll"
  };

}


