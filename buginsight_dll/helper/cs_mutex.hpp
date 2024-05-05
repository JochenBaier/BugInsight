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

//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#pragma once

#ifdef _WIN32
#include <Windows.h>
#else
#include <mutex>
#endif

struct cs_mutex_t
{
  cs_mutex_t();
  ~cs_mutex_t();

  void lock() noexcept
  {
#ifdef _WIN32
    EnterCriticalSection(&m_mutex);
#else
    m_mutex.lock();
#endif
  }

  void unlock() noexcept
  {
#ifdef _WIN32
    LeaveCriticalSection(&m_mutex);
#else
    m_mutex.unlock();
#endif
  }

#ifdef _WIN32
  CRITICAL_SECTION m_mutex;
#else
  std::mutex m_mutex;
#endif
};
