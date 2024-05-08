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

//Based on VLD, Copyright 2005-2017 VLD Team (LGPL-2.1)
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
