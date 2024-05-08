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

#include "dbghelp_api.hpp"

#include <cassert>
#include <mutex>

#include "cs_mutex.hpp"


//not yet working
#if 0
using SymInitialize_t=BOOL (WINAPI*)(HANDLE hProcess, PCSTR UserSearchPath, BOOL fInvadeProcess);
using SymSetOptions_t=DWORD(WINAPI*)(DWORD   SymOptions);
using SymFromAddr_t=BOOL(__stdcall*)(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol);
using ImageDirectoryEntryToDataEx_t=PVOID(WINAPI*)(PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, PULONG Size, PIMAGE_SECTION_HEADER* FoundHeader);
using SymGetModuleInfoW64_t=BOOL (WINAPI*)(HANDLE hProcess, DWORD64 qwAddr, PIMAGEHLP_MODULEW64 ModuleInfo);
using SymLoadModuleExW_t=DWORD64 (WINAPI*)(HANDLE hProcess, HANDLE hFile, PCWSTR ImageName, PCWSTR ModuleName, DWORD64 BaseOfDll, DWORD DllSize, PMODLOAD_DATA Data, DWORD Flags);
using EnumerateLoadedModulesW64_t=BOOL (WINAPI*)(HANDLE hProcess, PENUMLOADED_MODULES_CALLBACKW64 EnumLoadedModulesCallback, PVOID UserContext);

using SymGetLineFromAddr64_t=BOOL(WINAPI*) ( HANDLE hProcess, DWORD64 qwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line64 );


SymInitialize_t SymInitialize_fptr{};
SymSetOptions_t SymSetOptions_fptr{};
SymFromAddr_t SymFromAddr_fptr{};
ImageDirectoryEntryToDataEx_t ImageDirectoryEntryToDataEx_fptr{};
SymGetModuleInfoW64_t SymGetModuleInfoW64_fptr{};
SymLoadModuleExW_t SymLoadModuleExW_fptr{};
EnumerateLoadedModulesW64_t EnumerateLoadedModulesW64_fptr{};
SymGetLineFromAddr64_t SymGetLineFromAddr64_fptr{};
#endif

std::mutex g_mutex;

namespace dbghelp_api
{
void loaddll()
{
  //not yet working
#if 0
  HMODULE dbgdll_module=LoadLibraryA("dbghelp.dll");
  assert(dbgdll_module);

  SymInitialize_fptr=(SymInitialize_t)GetProcAddress(dbgdll_module, "SymInitialize");
  SymSetOptions_fptr=(SymSetOptions_t)GetProcAddress(dbgdll_module, "SymSetOptions");
  SymFromAddr_fptr=(SymFromAddr_t)GetProcAddress(dbgdll_module, "SymFromAddr");
  ImageDirectoryEntryToDataEx_fptr=(ImageDirectoryEntryToDataEx_t)GetProcAddress(dbgdll_module, "ImageDirectoryEntryToDataEx");
  SymGetModuleInfoW64_fptr=(SymGetModuleInfoW64_t)GetProcAddress(dbgdll_module, "SymGetModuleInfoW64");
  SymLoadModuleExW_fptr=(SymLoadModuleExW_t)GetProcAddress(dbgdll_module, "SymLoadModuleExW");
  EnumerateLoadedModulesW64_fptr=(EnumerateLoadedModulesW64_t)GetProcAddress(dbgdll_module, "EnumerateLoadedModulesW64");
  SymGetLineFromAddr64_fptr=(SymGetLineFromAddr64_t)GetProcAddress(dbgdll_module, "SymGetLineFromAddr64");
#endif
}

BOOL bi_syminitialize(HANDLE hProcess, PCSTR UserSearchPath, BOOL fInvadeProcess)
{
  std::lock_guard lock(g_mutex);
  
  //assert(SymInitialize_fptr);
  //return SymInitialize_fptr(hProcess, UserSearchPath,fInvadeProcess);
  return SymInitialize(hProcess, UserSearchPath,fInvadeProcess);
}

DWORD bi_symsetoptions(DWORD  SymOptions)
{
  std::lock_guard lock(g_mutex);

  //assert(SymSetOptions_fptr);
  //return SymSetOptions_fptr(SymOptions);
  return SymSetOptions(SymOptions);
}

BOOL bi_symfromaddr(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol)
{
  std::lock_guard lock(g_mutex);

  //assert(SymFromAddr_fptr);
  //return SymFromAddr_fptr(hProcess, Address, Displacement, Symbol);
  return SymFromAddr(hProcess, Address, Displacement, Symbol);
}

PVOID bi_imagedirectoryentrytodataex(PVOID Base, BOOLEAN MappedAsImage,USHORT DirectoryEntry, PULONG Size, PIMAGE_SECTION_HEADER* FoundHeader)
{
  std::lock_guard lock(g_mutex);

  //assert(ImageDirectoryEntryToDataEx_fptr);
  //return ImageDirectoryEntryToDataEx_fptr(Base, MappedAsImage, DirectoryEntry, Size, FoundHeader);
  return ImageDirectoryEntryToDataEx(Base, MappedAsImage, DirectoryEntry, Size, FoundHeader);
}

BOOL bi_symgetmoduleinfow64(HANDLE hProcess, DWORD64 qwAddr, PIMAGEHLP_MODULEW64 ModuleInfo)
{
  std::lock_guard lock(g_mutex);

  //assert(SymGetModuleInfoW64_fptr);
  //return SymGetModuleInfoW64_fptr(hProcess, qwAddr, ModuleInfo);
  return SymGetModuleInfoW64(hProcess, qwAddr, ModuleInfo);
}

DWORD64 bi_symloadmoduleexw(HANDLE hProcess, HANDLE hFile, PCWSTR ImageName, PCWSTR ModuleName, DWORD64 BaseOfDll, DWORD DllSize, PMODLOAD_DATA Data, DWORD Flags)
{
  std::lock_guard lock(g_mutex);

  //assert(SymLoadModuleExW_fptr);
  //return SymLoadModuleExW_fptr(hProcess, hFile, ImageName, ModuleName, BaseOfDll, DllSize, Data, Flags);
  return SymLoadModuleExW(hProcess, hFile, ImageName, ModuleName, BaseOfDll, DllSize, Data, Flags);
}

BOOL bi_enumerateloadedmodulesw64(HANDLE hProcess, PENUMLOADED_MODULES_CALLBACKW64 EnumLoadedModulesCallback, PVOID UserContext)
{
  std::lock_guard lock(g_mutex);

  //assert(EnumerateLoadedModulesW64_fptr);
  //return EnumerateLoadedModulesW64_fptr(hProcess, EnumLoadedModulesCallback, UserContext);
  return EnumerateLoadedModulesW64(hProcess, EnumLoadedModulesCallback, UserContext);
}

BOOL bi_symgetlinefromaddr64(HANDLE hProcess, DWORD64 qwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line64)
{
  std::lock_guard lock(g_mutex);

  //assert(SymGetLineFromAddr64_fptr);
  //return SymGetLineFromAddr64_fptr(hProcess, qwAddr, pdwDisplacement, Line64);
  return SymGetLineFromAddr64(hProcess, qwAddr, pdwDisplacement, Line64);
}


}
