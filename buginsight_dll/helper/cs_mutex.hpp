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
