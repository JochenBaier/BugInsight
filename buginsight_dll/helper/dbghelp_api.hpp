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

#pragma once

#include <Windows.h>
#include <dbghelp.h>

namespace dbghelp_api
{
void loaddll();

BOOL bi_syminitialize(HANDLE hProcess, PCSTR UserSearchPath, BOOL fInvadeProcess);
DWORD bi_symsetoptions(DWORD   SymOptions);
BOOL bi_symfromaddr(HANDLE hProcess, DWORD64 Address, PDWORD64 Displacement, PSYMBOL_INFO Symbol);
PVOID bi_imagedirectoryentrytodataex(PVOID Base, BOOLEAN MappedAsImage, USHORT DirectoryEntry, PULONG Size, PIMAGE_SECTION_HEADER* FoundHeader);
BOOL bi_symgetmoduleinfow64(HANDLE hProcess, DWORD64 qwAddr, PIMAGEHLP_MODULEW64 ModuleInfo);
DWORD64 bi_symloadmoduleexw(HANDLE hProcess, HANDLE hFile, PCWSTR ImageName, PCWSTR ModuleName, DWORD64 BaseOfDll, DWORD DllSize, PMODLOAD_DATA Data, DWORD Flags);
BOOL bi_enumerateloadedmodulesw64(HANDLE hProcess, PENUMLOADED_MODULES_CALLBACKW64 EnumLoadedModulesCallback, PVOID UserContext);
BOOL bi_symgetlinefromaddr64( HANDLE hProcess, DWORD64 qwAddr, PDWORD pdwDisplacement, PIMAGEHLP_LINE64 Line64);
}
