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
