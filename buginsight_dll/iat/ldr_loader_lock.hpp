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
