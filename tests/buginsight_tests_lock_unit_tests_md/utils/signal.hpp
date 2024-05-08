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

#include <condition_variable>
#include <mutex>

namespace utils
{
class event_t
{
public:
  std::cv_status wait_with_timeout(const int& p_milliseconds);
  std::cv_status wait();
  void notify_one();

private:
  std::mutex m_signal_mutex;
  std::condition_variable m_signal_condition;
  bool m_notified=false;
};

};