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
 
//BugInsight, Copyright 2024 Jochen Baier <email@jochen-baier.de>

#include "memory_leak_map_with_callstack.hpp"

#include <map>
#include <vector>
#include <algorithm>
#include <utility>

#include <boost/unordered/unordered_flat_set.hpp>
#include <boost/format.hpp>

#include "stacktrace.h"
#include "memory_helper.hpp"

#include <xxhasher.hpp>

TM_ATTRIBUTE_NOINLINE void memory_leak_repository_t::add(const uintptr_t& p_key, const size_t& p_size, api_family_t p_api_family, api_functions_t p_api_function)
{
  memory_leak_data_t leak_data_with_callstack(p_size, p_api_family, p_api_function);

  if(m_collect_callstacks)
  {
    leak_data_with_callstack.m_callstack.m_size=GetStackFrames(leak_data_with_callstack.m_callstack.m_ptr.data(), &leak_data_with_callstack.m_stackframe_hash, (int)leak_data_with_callstack.m_callstack.m_ptr.size(), 1, true);
  }

  bool ok=m_flat_map.emplace(std::make_pair(0, p_key), leak_data_with_callstack);
  boost::ignore_unused(ok);
  assert(ok);
}

TM_ATTRIBUTE_NOINLINE  void memory_leak_repository_t::add(const uintptr_t& p_heap, const uintptr_t& p_mem, const size_t& p_size, api_functions_t p_api_function)
{
  memory_leak_data_t leak_data_with_callstack(p_size, api_family_t::e_heapapi, p_api_function);

  if(m_collect_callstacks)
  {
    leak_data_with_callstack.m_callstack.m_size=GetStackFrames(leak_data_with_callstack.m_callstack.m_ptr.data(), &leak_data_with_callstack.m_stackframe_hash, (int)leak_data_with_callstack.m_callstack.m_ptr.size(), 1, true);
  }

  bool ok=m_flat_map.emplace(std::make_pair(p_heap, p_mem), leak_data_with_callstack);
  boost::ignore_unused(ok);
  assert(ok);
}

void memory_leak_repository_t::erase(const uintptr_t& p_key)
{
  const size_t count=m_flat_map.erase(std::make_pair(0, p_key));
  boost::ignore_unused(count);
  assert(count == 1);
}

void memory_leak_repository_t::erase(const uintptr_t& p_heap, const uintptr_t& p_key)
{
  const size_t count=m_flat_map.erase(std::make_pair(p_heap, p_key));
  boost::ignore_unused(count);
  assert(count == 1);
}

bool memory_leak_repository_t::get_data(const uintptr_t& p_key, api_family_t& p_api_family, boost::span<uint8_t>& p_user_memory, api_functions_t& p_api_function)
{
  const size_t visit_count=m_flat_map.visit(std::make_pair(0, p_key), [&](const auto& x)
    {
      const memory_leak_data_t& alloc_data=x.second;
      p_api_family=alloc_data.m_api_family;
      p_user_memory=boost::span<uint8_t>((uint8_t*)p_key, alloc_data.m_size);
      p_api_function=alloc_data.m_api_function;

    });

  return visit_count == 1;
}

bool memory_leak_repository_t::get_data(const uintptr_t& p_heap, const uintptr_t& p_key, api_family_t& p_api_family, boost::span<uint8_t>& p_user_memory, api_functions_t& p_api_function)
{
  const size_t visit_count=m_flat_map.visit(std::make_pair(p_heap, p_key), [&](const auto& x)
    {
      const memory_leak_data_t& alloc_data=x.second;
      p_api_family=alloc_data.m_api_family;
      p_user_memory=boost::span<uint8_t>((uint8_t*)p_key, alloc_data.m_size);
      p_api_function=alloc_data.m_api_function;

    });

  return visit_count == 1;
}

void memory_leak_repository_t::generate_leak_report2(fmt::memory_buffer& p_log_string, const std::vector<used_api_info_t>& p_used_api_infos)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\nMemory leak report:\n\n");
  fmt::format_to(std::back_inserter(p_log_string), "{:30}|{:>10}|{:>20}\n", "Function", "Calls", "Bytes allocated");
  fmt::format_to(std::back_inserter(p_log_string), "----------------------------------------------------------------\n");

  for(const auto& api_info : p_used_api_infos)
  {
    if(!api_info.m_hit_count)
    {
      continue;
    }

    std::string function_name;
    function_name=api_function_to_string(api_info.m_api_function);
    fmt::format_to(std::back_inserter(p_log_string), "{:30}|{:>10}|{:>20}\n", function_name, api_info.m_hit_count, api_info.m_total_bytes);
  }
}

void memory_leak_repository_t::generate_callstacks2(fmt::memory_buffer& p_log_string, const std::vector<used_api_info_t>& p_used_api_infos,
  const std::vector<uint64_t>& p_ids_to_suppress, bool& p_leaks_suppressed, const modules_static_runtime_t& p_modules_static_runtime)
{
  fmt::format_to(std::back_inserter(p_log_string), "\n\nMemory leak callstacks:\n");

  for(const auto& used_api_info : p_used_api_infos)
  {
    if(!used_api_info.m_hit_count)
    {
      continue;
    }

    fmt::format_to(std::back_inserter(p_log_string), "\n\n[Function: '{}', Hit count: {}, Sum bytes allocated: {}]\n",
      api_function_to_string(used_api_info.m_api_function), used_api_info.m_hit_count, used_api_info.m_total_bytes);

    for(const auto& callstack_and_size : used_api_info.m_callstack_and_size)
    {
      if(!callstack_and_size.m_size_bytes)
      {
        continue;
      }

      auto callstack_buffer=fmt::memory_buffer();
      xxhasher_t callstack_hash;
      printf_call_stack_symbol_infos(callstack_and_size.m_callstack, callstack_buffer, callstack_hash, p_modules_static_runtime);

      const uint64_t hashcode=callstack_hash.create_hash();

      if(std::find(p_ids_to_suppress.begin(), p_ids_to_suppress.end(), hashcode) != p_ids_to_suppress.end())
      {
        p_leaks_suppressed=true;
        continue;
      }

      //not yet public const std::string hash_hex=(boost::format("%x") % hashcode).str();
      fmt::format_to(std::back_inserter(p_log_string), "\nCall Stack:\n");

      p_log_string.append(callstack_buffer);
    }
  }

}

bool memory_leak_repository_t::group_leaks2(std::vector<used_api_info_t>& p_used_api_infos)
{
  const size_t visit_count=m_flat_map.visit_all([&](const auto& x)
    {
      memory_leak_data_t memory_leak_data=x.second;
      unsigned long& api_callstack_hash=memory_leak_data.m_stackframe_hash;

      assert((size_t)memory_leak_data.m_api_function < p_used_api_infos.size());
      used_api_info_t& used_api_info=p_used_api_infos[(size_t)memory_leak_data.m_api_function];
      ++used_api_info.m_hit_count;
      used_api_info.m_total_bytes+=memory_leak_data.m_size;

      size_t found_index=0;
      bool found_via_hash=false;

      for(size_t i=0; i < used_api_info.m_callstack_and_size.size(); ++i)
      {
        callstack_size_hash_t& callstack_size_hash=used_api_info.m_callstack_and_size[i];
        if(callstack_size_hash.m_hash == api_callstack_hash)
        {
          found_via_hash=true;
          if(memory_leak_data.m_size > callstack_size_hash.m_size_bytes)
          {
            callstack_size_hash.m_size_bytes=memory_leak_data.m_size;
          }
          break;
        }

        if(callstack_size_hash.m_size_bytes < used_api_info.m_callstack_and_size[found_index].m_size_bytes)
        {
          found_index=i;
        }
      }

      auto& callstack_size_hash=used_api_info.m_callstack_and_size[found_index];
      if(!found_via_hash && memory_leak_data.m_size > callstack_size_hash.m_size_bytes)
      {
        callstack_size_hash.m_hash=memory_leak_data.m_stackframe_hash;
        callstack_size_hash.m_size_bytes=memory_leak_data.m_size;
        callstack_size_hash.m_callstack=memory_leak_data.m_callstack;
      }
    });

  std::sort(p_used_api_infos.begin(), p_used_api_infos.end(), [](const used_api_info_t& a, const used_api_info_t& b)
    {
      return a.m_total_bytes > b.m_total_bytes;
    });

  return visit_count;
}

size_t memory_leak_repository_t::buginsight_internal_get_allocated_bytes(const std::string& p_function)
{
  size_t sum_allocated_bytes=0;

  api_functions_t api_function=api_functions_t::e_unknown;

  if(p_function != "*")
  {
    api_function=api_function_string_to_enum(p_function);
    assert(api_function != api_functions_t::e_unknown);
  }

  m_flat_map.visit_all([&](const auto& x)
    {
      const memory_leak_data_t memory_leak_data=x.second;
      if(api_function != api_functions_t::e_unknown && memory_leak_data.m_api_function != api_function)
      {
        return;
      }

      const size_t size=memory_leak_data.m_size;
      sum_allocated_bytes+=size;
    });

  return sum_allocated_bytes;
}

void memory_leak_repository_t::internal_reset_all_allocations()
{
  m_flat_map.clear();
}



