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


#include "stacktrace.h"
#include "leak_types.hpp"

#ifdef _WIN32
#include <windows.h>
#pragma warning(push)
#pragma warning(disable:4091)
#pragma warning(pop)
#endif

#include <cassert>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/core/ignore_unused.hpp>

#include "modules_helper.hpp"
#include "xxhasher.hpp"
#include "log.hpp"
#include "dbghelp_api.hpp"

typedef USHORT NTAPI RtlCaptureStackBackTrace_Function(
  IN ULONG frames_to_skip,
  IN ULONG frames_to_capture,
  OUT PVOID* backtrace,
  OUT PULONG backtrace_hash);

HANDLE process=nullptr;
HMODULE g_buginsight_dll_module=nullptr;

TM_ATTRIBUTE_NOINLINE int GetStackFrames(void** result, unsigned long* p_hash, int p_max_depth, int p_skip_count, bool p_check)
{
  static RtlCaptureStackBackTrace_Function* const RtlCaptureStackBackTrace_fn=(RtlCaptureStackBackTrace_Function*)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlCaptureStackBackTrace");

  assert(RtlCaptureStackBackTrace_fn);
  const int size=(RtlCaptureStackBackTrace_fn)(static_cast<ULONG>(p_skip_count) + 1, static_cast<ULONG>(p_max_depth), result, p_hash);

#ifdef _DEBUG
  if(p_check)
  {
    assert(size > 1);
    HMODULE hCallingModule=modules_helper::get_calling_module((UINT_PTR)result[0]);
    if(!hCallingModule)
    {
      assert(false);
    }

    if(hCallingModule != g_buginsight_dll_module)
    {
      assert(false);
    }

    hCallingModule=modules_helper::get_calling_module((UINT_PTR)result[1]);
    if(!hCallingModule)
    {
      assert(false);
    }

    if(hCallingModule == g_buginsight_dll_module)
    {
      assert(false);
    }

  }
#else
  boost::ignore_unused(p_check);
#endif

  return size;
}

void InitializeSymbolizer()
{
#if _WIN32
  if(process != nullptr)
  {
    return;
  }
  process=GetCurrentProcess();


  if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPCSTR)&InitializeSymbolizer, &g_buginsight_dll_module) == 0)
  {
    assert(false);
  }
  assert(g_buginsight_dll_module);

  // Symbols are not loaded until a reference is made requiring the
  // symbols be loaded. This is the fastest, most efficient way to use
  // the symbol handler.
  dbghelp_api::bi_symsetoptions(SYMOPT_DEFERRED_LOADS | SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
  if(!dbghelp_api::bi_syminitialize(process, nullptr, false))
  {
    const unsigned long long error{GetLastError()};  // NOLINT(runtime/int)
    LOG(critical, "BugInsight: Internal error: SymInitialize() failed: %llu", error);
    assert(false);
  }
#endif
}

void printf_symbolinfo(const size_t& p_index, const void* pc, fmt::memory_buffer& p_callstack_string, const xxhasher_t& p_callstack_hash, const modules_static_runtime_t& p_modules_static_runtime)
{
  assert(pc);

  static const std::string static_runtime_warning=" [Warning: static runtime!]";
  bool has_static_runtime=false;

  const HMODULE hCallingModule=modules_helper::get_calling_module((UINT_PTR)pc);
  if(!hCallingModule)
  {
    assert(p_index != 0);
    return;
  }

  //First is inside buginsight. ignore
  if(p_index == 0)
  {
    if(hCallingModule != g_buginsight_dll_module)
    {
      LOG(err, "printf_symbolinfo: BugInsight DLL is not on first index");
      assert(false);
    }
    return;
  }

  assert((uintptr_t)pc > (uintptr_t)hCallingModule);
  const ptrdiff_t module_offset=(uintptr_t)pc - (uintptr_t)hCallingModule;

  std::string module_name="---";
  char callingModuleName[2048]{};
  if(GetModuleFileName(hCallingModule, callingModuleName, _countof(callingModuleName)) > 0)
  {
    boost::filesystem::path p(callingModuleName);
    module_name=p.filename().string();

    p_callstack_hash.add(module_name.c_str(), module_name.size());
    has_static_runtime=p_modules_static_runtime.contains(boost::to_lower_copy(module_name));
  }

  alignas(SYMBOL_INFO) char buf[sizeof(SYMBOL_INFO) + MAX_SYM_NAME]{};
  SYMBOL_INFO* symbol=reinterpret_cast<SYMBOL_INFO*>(buf);
  symbol->SizeOfStruct=sizeof(SYMBOL_INFO);
  symbol->MaxNameLen=MAX_SYM_NAME;

  if(!dbghelp_api::bi_symfromaddr(process, reinterpret_cast<DWORD64>(pc), nullptr, symbol))
  {
    fmt::format_to(std::back_inserter(p_callstack_string), "{} ({}) (no symbol info){}\n", pc, module_name, has_static_runtime?static_runtime_warning:"");
    return;
  }

  const std::string symbol_name=symbol->Name;

  p_callstack_hash.add(&module_offset, sizeof(module_offset));

  // Get the line number information
  IMAGEHLP_LINE64 line;
  line.SizeOfStruct=sizeof(IMAGEHLP_LINE64);

  DWORD displacement;
  const bool get_line_ok= dbghelp_api::bi_symgetlinefromaddr64(process, reinterpret_cast<DWORD64>(pc), &displacement, &line);

  fmt::format_to(std::back_inserter(p_callstack_string), "{}({},0): {} ({}){}\n", get_line_ok?line.FileName:"---",
    get_line_ok?line.LineNumber:0, symbol_name, module_name.c_str(), has_static_runtime?static_runtime_warning:"");
}

void printf_call_stack_symbol_infos(const callstack_t& p_stack_frame, fmt::memory_buffer& p_callstack_string, const xxhasher_t& p_callstack_hash, const modules_static_runtime_t& p_modules_static_runtime)
{
  for(size_t i=0; i < p_stack_frame.m_size; i++)
  {
    printf_symbolinfo(i, p_stack_frame.m_ptr[i], p_callstack_string, p_callstack_hash, p_modules_static_runtime);
  }
}

TM_ATTRIBUTE_NOINLINE void save_current_callstack_to_string(fmt::memory_buffer& p_callsstack_string, const int& p_skip_count)
{
#if _WIN32
  unsigned long hash_unused=0;
  callstack_t stack_frame;
  stack_frame.m_size=GetStackFrames(stack_frame.m_ptr.data(), &hash_unused, (int)stack_frame.m_ptr.size(), p_skip_count, true);

  xxhasher_t callstack_hash_not_used;
  modules_static_runtime_t modules_static_runtime_not_used;
  printf_call_stack_symbol_infos(stack_frame, p_callsstack_string, callstack_hash_not_used, modules_static_runtime_not_used);
#endif
}

