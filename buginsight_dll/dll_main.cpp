//This is an open source non-commercial project. Dear PVS-Studio, please check it.

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


#include <Windows.h>

#include <intrin.h>

#define _DECL_DLLMAIN  // for _CRT_INIT

#include <psapi.h>
#include <boost/core/ignore_unused.hpp>

#include "process.h"   // for _CRT_INIT
#include "ldr_loader_lock.hpp"
#include "loader.hpp"
#include <detours/detours.h>
#include "stacktrace.h"
#include "hex_dump.hpp"

loader_t g_loader;
static HANDLE g_current_module=GetCurrentProcess();

typedef BOOLEAN(NTAPI* PDLL_INIT_ROUTINE)(IN PVOID DllHandle, IN ULONG Reason, IN PCONTEXT Context OPTIONAL);
BOOLEAN NTAPI MyLdrpCallInitRoutine(IN PDLL_INIT_ROUTINE  	EntryPoint, IN PVOID  	BaseAddress, IN ULONG  	Reason, IN PVOID  	Context);
using LdrpCallInitRoutine_t=BOOLEAN(NTAPI*) (PDLL_INIT_ROUTINE EntryPoint, PVOID, ULONG, PVOID);
LdrpCallInitRoutine_t TrueLdrpCallInitRoutine{};

__declspec(noinline)
void dll_process_attach();
void dll_process_detach();

__declspec(noinline)
BOOL WINAPI DllEntryPoint(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
  // Patch/Restore ntdll address that calls the dll entry point
  if(fdwReason == DLL_PROCESS_ATTACH)
  {
    try
    {
      printf("[%s] [info] Loading '%s' into application under test.\n", std::string(APP_NAME_PRETTY).c_str(), std::string(APP_NAME_PRETTY).c_str());
      fflush(stdout);  // NOLINT(cert-err33-c)

      dll_process_attach();
    } catch(std::runtime_error& p_ex)
    {
      const std::string error=fmt::format("Fatal error during monitor DLL loading: '{}'.\n", p_ex.what());
      printf("[%s] [fatal] %s", APP_NAME_PRETTY, error.c_str());  // NOLINT(cert-err33-c)
      fflush(stdout); // NOLINT(cert-err33-c)

      MessageBoxA(nullptr, error.c_str(), APP_NAME_PRETTY, MB_OK | MB_ICONERROR);
      return false;
    }
  }

  if(fdwReason == DLL_PROCESS_ATTACH || fdwReason == DLL_THREAD_ATTACH)
  {
    if(!_CRT_INIT(hinstDLL, fdwReason, lpReserved))
    {
      return(FALSE);
    }
  }

  if(fdwReason == DLL_PROCESS_DETACH || fdwReason == DLL_THREAD_DETACH)
  {
    if(!_CRT_INIT(hinstDLL, fdwReason, lpReserved))
    {
      return(FALSE);
    }
  }

  if(fdwReason == DLL_PROCESS_DETACH)
  {
    dll_process_detach();
  }
  return(TRUE);
}

// (vld) We provide our own DllEntryPoint in order to capture the ReturnAddress of the function calling our DllEntryPoint.
// Then we patch this function namely ntdll.dll!LdrpCallInitRoutine or any equivalent NT loader function by calling
// our LdrpCallInitRoutine where we RefreshModules before calling the EntryPoint in order to hook all functions required
// to properly capture all _CRT_INIT memory allocations which include internal CRT startup memory allocations and all
// global and static initializers.
BOOLEAN NTAPI MyLdrpCallInitRoutine(IN PDLL_INIT_ROUTINE  	EntryPoint, IN PVOID  	BaseAddress, IN ULONG  	Reason, IN PVOID  	Context)
{
  if(g_loader.is_global_blacklisted_module((HMODULE)BaseAddress))
  {
    return TrueLdrpCallInitRoutine(EntryPoint, BaseAddress, Reason, Context);
  }

  wchar_t modulePath[2048]{};
  if(!GetModuleFileNameExW(g_current_module, (HMODULE)BaseAddress, modulePath, sizeof(modulePath) / sizeof(wchar_t)))
  {
    return TrueLdrpCallInitRoutine(EntryPoint, BaseAddress, Reason, Context);
  }

  if(modules_helper::module_has_microsoft_copyright(modulePath))
  {
    return TrueLdrpCallInitRoutine(EntryPoint, BaseAddress, Reason, Context);
  }

  assert(g_loader.m_init_finished);

  ldr_loader_lock_t ll;

  if(Reason == DLL_PROCESS_ATTACH)
  {
    g_loader.patch_loaded_modules();
  }

  return TrueLdrpCallInitRoutine(EntryPoint, BaseAddress, Reason, Context);
}

PBYTE find_ldrpcallinitroutine_address(PBYTE p_LdrpInitializeNode_frame)
{
  //https://stackoverflow.com/questions/10376787/need-help-understanding-e8-asm-call-instruction-x86
  //00007FFBAC0FD038 E8 [F7 B5 FB FF]     call     LdrpCallInitRoutine(07FFBAC0B8634h) ,=
  //00007FFBAC0FD03D 88 44 24 40          mov         byte ptr[rsp + 40h], al  <--- p_LdrpInitializeNode_frame

  const PBYTE call_op_code=p_LdrpInitializeNode_frame - 5;
  if(call_op_code[0] != 0xe8)
  {
    const std::string hexstring=hex_dump::to_string<5, false>((void*)call_op_code, 20);

    const std::string error=fmt::format("Did not found expected assembly instruction 'call with relative offset (0xE8)' at address '{}'. Dump:\n\n{}", (void*)p_LdrpInitializeNode_frame, hexstring);
    throw std::runtime_error(error);
  }

  const void* offset_start=call_op_code + 1;
  const int32_t call_offset=*(const int32_t* const)offset_start;

  const PBYTE LdrpCallInitRoutine_address=p_LdrpInitializeNode_frame + call_offset;
  return LdrpCallInitRoutine_address;
}

__declspec(noinline)
void dll_process_attach()
{
  PBYTE LdrpInitializeNode_frame{};
  unsigned long hash{};
  const auto n=GetStackFrames((void**)&LdrpInitializeNode_frame, &hash, 1, 3, false);
  if(n != 1)
  {
    throw std::runtime_error("Failed to capture stack trace from entry point");
  }

  const PBYTE LdrpCallInitRoutine_address=find_ldrpcallinitroutine_address(LdrpInitializeNode_frame);
  assert(LdrpCallInitRoutine_address);

  TrueLdrpCallInitRoutine=(LdrpCallInitRoutine_t)LdrpCallInitRoutine_address;

  assert(!DetourIsHelperProcess());

  LONG error=DetourTransactionBegin();
  if(error != NO_ERROR)
  {
    assert(false);
    const std::string error_string=fmt::format("Function 'DetourTransactionBegin()' failed: '{}'", error);
    throw std::runtime_error(error_string);
  }

  error=DetourUpdateThread(GetCurrentThread());
  if(error != NO_ERROR)
  {
    assert(false);
    const std::string error_string=fmt::format("Function 'DetourUpdateThread()' failed: '{}'", error);
    throw std::runtime_error(error_string);
  }

  error=DetourAttach(&(PVOID&)TrueLdrpCallInitRoutine, MyLdrpCallInitRoutine);
  if(error != NO_ERROR)
  {
    assert(false);
    const std::string error_string=fmt::format("Function 'DetourAttach()' failed: '{}'", error);
    throw std::runtime_error(error_string);
  }

  error=DetourTransactionCommit();
  if(error != NO_ERROR)
  {
    assert(false);
    const std::string error_string=fmt::format("Function 'DetourTransactionCommit()' failed: '{}'", error);
    throw std::runtime_error(error_string);
  }
}

void dll_process_detach()
{
  LONG error=DetourTransactionBegin();
  assert(error == NO_ERROR);

  error=DetourUpdateThread(GetCurrentThread());
  boost::ignore_unused(error);
  assert(error == NO_ERROR);

  error=DetourDetach(&(PVOID&)TrueLdrpCallInitRoutine, MyLdrpCallInitRoutine);
  boost::ignore_unused(error);
  assert(error == NO_ERROR);

  error=DetourTransactionCommit();
  boost::ignore_unused(error);
  assert(error == NO_ERROR);
}


