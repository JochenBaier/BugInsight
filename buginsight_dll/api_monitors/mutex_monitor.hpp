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

#include <cassert>
#include <cstdint>

#include "deadlock_monitor.hpp"
#include "run_time_options.hpp"

extern deadlock_monitor_t* g_deadlock_monitor;
extern statistics_t g_statistics;
extern std::atomic_uint64_t g_monitored_api_calls;

using _Mtx_lock_t=_Thrd_result(__cdecl*)(_Mtx_t) noexcept;
using _Mtx_unlock_t=_Thrd_result(__cdecl*)(_Mtx_t) noexcept;
using _Mtx_trylock_t=_Thrd_result(__cdecl*)(_Mtx_t) noexcept;
using _Mtx_init_in_situ_t=void(__cdecl*)(_Mtx_t, int) noexcept;
using _Mtx_destroy_in_situ_t=void(__cdecl*)(_Mtx_t) noexcept;
using _Cnd_wait_t=_Thrd_result(__cdecl*)(_Cnd_t, _Mtx_t) noexcept;
using _Cnd_timedwait_t=int(__cdecl*) (_Cnd_t c, _Mtx_t t, const void* d);

template<bool debug>
class mutex_monitor_t
{
public:
  inline static _Mtx_lock_t _Mtx_lock_fptr{};
  static int bi_mtx_lock(_Mtx_t t)
  {
    ++g_monitored_api_calls;

    void* hold_locks=nullptr;
    assert(g_deadlock_monitor);

    uint64_t id=0;
    g_deadlock_monitor->dlc_deadlock_check_before_lock(t, &hold_locks, id);
    if(id == 0)
    {
      return static_cast<int>(_Mtx_lock_fptr(t));
    }

    const int res=static_cast<int>(_Mtx_lock_fptr(t));
    g_deadlock_monitor->dlc_deadlock_check_in_lock(t, id, hold_locks);

    return res;
  }

  inline static _Mtx_unlock_t _Mtx_unlock_fptr{};
  static int bi_mtx_unlock(_Mtx_t t)
  {
    ++g_monitored_api_calls;

    g_deadlock_monitor->dlc_deadlock_check_after_lock(t);
    return static_cast<int>(_Mtx_unlock_fptr(t));
  }

  inline static _Mtx_trylock_t _Mtx_trylock_fptr{};
  static int bi_mtx_trylock(_Mtx_t t)
  {
    ++g_monitored_api_calls;

    void* hold_locks=nullptr;
    uint64_t id=0;
    g_deadlock_monitor->dlc_deadlock_check_before_try_lock(t, &hold_locks, id);
    if(id == 0)
    {
      assert(false);
      return static_cast<int>(_Mtx_trylock_fptr(t));
    }

    const int res=static_cast<int>(_Mtx_trylock_fptr(t));
    if(res != 0)
    {
      return res;
    }

    g_deadlock_monitor->dlc_deadlock_check_in_lock(t, id, hold_locks);
    return res;
  }

  inline static _Mtx_init_in_situ_t _Mtx_init_in_situ_fptr{};
  static void bi_init_in_situ(_Mtx_t t, int i)
  {
    ++g_monitored_api_calls;

    _Mtx_init_in_situ_fptr(t, i);
  }

  inline static _Mtx_destroy_in_situ_t _Mtx_destroy_in_situ_fptr{};
  static void bi_mtx_destroy_in_situ(_Mtx_t t)
  {
    ++g_monitored_api_calls;

    g_deadlock_monitor->dlc_deadlock_delete_lock(t);
    _Mtx_destroy_in_situ_fptr(t);
  }

  inline static _Cnd_wait_t _Cnd_wait_fptr{};
  static int bi__cnd_wait(_Cnd_t c, _Mtx_t t)
  {
    ++g_monitored_api_calls;

    g_deadlock_monitor->buginsight_cnd_wait(t);
    return static_cast<int>(_Cnd_wait_fptr(c, t));
  }

  inline static _Cnd_timedwait_t _Cnd_timedwait_fptr{};
  static int bi__cnd_timedwait(_Cnd_t c, _Mtx_t t, const void* d)
  {
    ++g_monitored_api_calls;

    g_deadlock_monitor->buginsight_cnd_wait(t);
    return static_cast<int>(_Cnd_timedwait_fptr(c, t, d));
  }

};
