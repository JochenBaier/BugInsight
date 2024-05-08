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
#include "leak_types.hpp"

#include "fmt/format.h"

#include "boost/unordered/concurrent_flat_map.hpp"
#include <boost/unordered/unordered_flat_map.hpp>
#include <boost/core/span.hpp>

#include "attributes.hpp"
#include "modules_helper.hpp"

struct callstack_size_hash_t
{
  callstack_t m_callstack;
  size_t m_size_bytes{};
  unsigned long m_hash{};
};

struct used_api_info_t
{
  used_api_info_t()
  {
    m_callstack_and_size.resize(10); //collect max 10 callstack per API
  }

  std::vector<callstack_size_hash_t> m_callstack_and_size;

  size_t m_total_bytes{0};
  api_functions_t m_api_function=api_functions_t::e_unknown;
  size_t m_hit_count{0};
};

class memory_leak_repository_t
{
public:
  memory_leak_repository_t(const bool& p_collect_callstacks) :
    m_collect_callstacks(p_collect_callstacks)
  {
    m_flat_map.reserve(50000);
  }

  TM_ATTRIBUTE_NOINLINE void add(const uintptr_t& p_key, const size_t& p_size, api_family_t p_api_family, api_functions_t p_api_function);
  TM_ATTRIBUTE_NOINLINE void add(const uintptr_t& p_heap, const uintptr_t& p_mem, const size_t& p_size, api_functions_t p_api_function);

  void erase(const uintptr_t& p_key);
  void erase(const uintptr_t& p_heap, const uintptr_t& p_key);

  bool get_data(const uintptr_t& p_key, api_family_t& p_api_family, boost::span<uint8_t>& p_user_memory, api_functions_t& p_api_function);
  bool get_data(const uintptr_t& p_heap, const uintptr_t& p_key, api_family_t& p_api_family, boost::span<uint8_t>& p_user_memory, api_functions_t& p_api_function);

  static void generate_callstacks2(fmt::memory_buffer& p_log_string, const std::vector<used_api_info_t>& p_used_api_infos, const std::vector<uint64_t>& p_ids_to_suppress, bool& p_leaks_suppressed, const modules_static_runtime_t& p_modules_static_runtime);
  bool group_leaks2(std::vector<used_api_info_t>& p_used_api_infos);
  static void generate_leak_report2(fmt::memory_buffer& p_log_string, const std::vector<used_api_info_t>& p_used_api_infos);

  size_t buginsight_internal_get_allocated_bytes(const std::string& p_function);
  void internal_reset_all_allocations();

  size_t leak_count() const
  {
    return m_flat_map.size();
  }

private:

  bool m_collect_callstacks;

  struct memory_leak_data_t
  {
    memory_leak_data_t(const size_t& p_size, const api_family_t& p_api_family, const api_functions_t& p_api_function) :
      m_size(p_size),
      m_api_family(p_api_family),
      m_api_function(p_api_function)
    {
    }

    size_t m_size;
    api_family_t m_api_family;
    api_functions_t m_api_function;

    callstack_t m_callstack;
    unsigned long m_stackframe_hash{0};
  };

  boost::concurrent_flat_map<std::pair<uintptr_t, uintptr_t>, memory_leak_data_t> m_flat_map;
};
