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

//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>


#include "one_shot_timer.hpp"
#include <cassert>
#include <cstdio>

#include "log.hpp"
#include "attributes.hpp"

class one_shot_timer_t;

#ifdef _WIN32
VOID CALLBACK TimerRoutine(PVOID lpParam, BOOLEAN TimerOrWaitFired)
{
  UNUSED(TimerOrWaitFired)
    assert(lpParam != nullptr);
  assert(TimerOrWaitFired);

  const one_shot_timer_t* one_shot_timer=static_cast<one_shot_timer_t*>(lpParam);
  one_shot_timer->timer_expired();
}
#endif

one_shot_timer_t::one_shot_timer_t(one_shot_timer_callback_t& p_one_shot_timer_callback) :
  m_one_shot_timer_callback(p_one_shot_timer_callback)
#ifdef __linux__
  , m_loop_thread(&one_shot_timer_t::loop, this)
#endif
{
#ifdef _WIN32
  // Create the timer queue.
  hTimerQueue=CreateTimerQueue();
  if(nullptr == hTimerQueue)
  {
    assert(false);
  }
#endif
}

one_shot_timer_t::~one_shot_timer_t()
{
#ifdef _WIN32
  assert(hTimerQueue);
  if(!DeleteTimerQueue(hTimerQueue))
  {
    assert(false);
  }
  hTimerQueue=nullptr;
#else
  m_shutdown=true;
  m_loop_thread.join();
#endif
}

void one_shot_timer_t::start(uint8_t p_duration_sec)
{
#if _WIN32
  if(!CreateTimerQueueTimer(&hTimer, hTimerQueue, TimerRoutine, this, p_duration_sec * 1000, 0, 0))
  {
    LOG(critical, "Internal error: CreateTimerQueueTimer failed (%lu)\n", GetLastError());
    assert(false);
  }
#else
  const std::lock_guard<cs_mutex_t> lock(m_mutex);
  assert(m_start_time_point.time_since_epoch().count() == 0);
  m_start_time_point=std::chrono::steady_clock::now();
#endif
}

void one_shot_timer_t::timer_expired() const
{
  m_one_shot_timer_callback.one_shot_timer_callback();
}

#ifdef __linux__
void one_shot_timer_t::loop()
{
  while(!m_shutdown)
  {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    {//scope
      const std::lock_guard<cs_mutex_t> lock(m_mutex);
      if(m_start_time_point.time_since_epoch().count() == 0)
      {
        continue;
      }
    }

    { //scope
      const std::lock_guard<cs_mutex_t> lock(m_mutex);
      const auto now=std::chrono::steady_clock::now();
      const uint64_t elapsed_ms=std::chrono::duration_cast<std::chrono::milliseconds>(now - m_start_time_point).count();
      if(elapsed_ms < 5 * 1000)
      {
        continue;
      }
      m_start_time_point={};
    }

    m_one_shot_timer_callback.one_shot_timer_callback();

  }
}
#endif

