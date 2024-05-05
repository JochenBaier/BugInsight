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


#include "ldr_loader_lock.hpp"

#include <cassert>

//
// LdrLockLoaderLock Flags
//
#define LDR_LOCK_LOADER_LOCK_FLAG_DEFAULT           0x00000000
//
// LdrUnlockLoaderLock Flags
//
#define LDR_UNLOCK_LOADER_LOCK_FLAG_DEFAULT         0x00000000


extern LdrLockLoaderLock_t LdrLockLoaderLock;
extern LdrUnlockLoaderLock_t LdrUnlockLoaderLock;

ldr_loader_lock_t::ldr_loader_lock_t()
{
  assert(LdrLockLoaderLock);
  ULONG uState=NULL;
  NTSTATUS ntStatus=LdrLockLoaderLock(LDR_LOCK_LOADER_LOCK_FLAG_DEFAULT, &uState, &m_uCookie);
  assert(ntStatus == STATUS_SUCCESS);
  if(ntStatus == STATUS_SUCCESS)
  {
    m_hThreadId=GetCurrentThreadId();
  }
}

ldr_loader_lock_t::~ldr_loader_lock_t()
{
  assert(LdrUnlockLoaderLock);
  if(m_uCookie)
  {
    NTSTATUS ntStatus=LdrUnlockLoaderLock(LDR_UNLOCK_LOADER_LOCK_FLAG_DEFAULT, m_uCookie);
    assert(ntStatus == STATUS_SUCCESS);
    if(ntStatus == STATUS_SUCCESS)
    {
      m_hThreadId=NULL;
      m_uCookie=NULL;
    }
  }
}
