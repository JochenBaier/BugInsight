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

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

// Return code type used by LdrLoadDll.
typedef __success(return >= 0) LONG NTSTATUS;
#define STATUS_SUCCESS 0

typedef NTSTATUS(NTAPI* LdrLockLoaderLock_t)(ULONG, PULONG, PULONG_PTR);
typedef NTSTATUS(NTAPI* LdrUnlockLoaderLock_t)(ULONG, ULONG_PTR);

class ldr_loader_lock_t
{
public:
  ldr_loader_lock_t();
  ~ldr_loader_lock_t();

  ldr_loader_lock_t(const ldr_loader_lock_t&)=delete;
  ldr_loader_lock_t& operator=(const ldr_loader_lock_t&)=delete;

private:
  DWORD m_hThreadId{};
  ULONG_PTR m_uCookie{};
};
