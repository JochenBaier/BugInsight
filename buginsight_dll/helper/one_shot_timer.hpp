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

#include <cstdint>

#ifdef __linux__
#include <thread>
#include <atomic>
#include "cs_mutex.hpp"
#else
#include <Windows.h>
#endif

class one_shot_timer_callback_t
{
public:
  virtual ~one_shot_timer_callback_t()=default;
  virtual void  one_shot_timer_callback()=0;
};

class one_shot_timer_t
{
public:
  one_shot_timer_t(one_shot_timer_callback_t& p_one_shot_timer_callback);
  ~one_shot_timer_t();
  void start(uint8_t p_duration_sec);
  void timer_expired() const;

private:
  one_shot_timer_callback_t& m_one_shot_timer_callback;

#ifdef _WIN32
  HANDLE hTimer=nullptr;
  HANDLE hTimerQueue=nullptr;
#else
  void loop();
  cs_mutex_t m_mutex;
  std::atomic_bool m_shutdown{false};
  std::chrono::steady_clock::time_point m_start_time_point{};
  std::thread m_loop_thread;
#endif
};

