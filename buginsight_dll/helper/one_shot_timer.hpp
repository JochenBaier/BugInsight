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

