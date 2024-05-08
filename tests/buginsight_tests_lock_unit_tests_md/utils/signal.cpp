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

#include "signal.hpp"
#include <cassert>

namespace utils
{
std::cv_status event_t::wait_with_timeout(const int& p_milliseconds)
{
  std::unique_lock lock(m_signal_mutex);
  const bool is_notified=m_signal_condition.wait_for(lock, std::chrono::milliseconds(p_milliseconds), [this]
    {
      return m_notified;
    });

  if(!is_notified)
  {
    return std::cv_status::timeout;
  }

  assert(m_notified);
  m_notified=false;
  return std::cv_status::no_timeout;
}

std::cv_status event_t::wait()
{
  std::unique_lock<std::mutex> lock(m_signal_mutex);
  m_signal_condition.wait(lock, [this]
    {
      return m_notified;
    });

  assert(m_notified);
  m_notified=false;
  return std::cv_status::no_timeout;
}

void event_t::notify_one()
{
  {//scope
    std::unique_lock lock(m_signal_mutex);
    m_notified=true;
  }

  m_signal_condition.notify_one();
}
}
