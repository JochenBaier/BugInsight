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

#include <iostream>
#include <ostream>
#include <thread>
#include <vector>

#include "allocator.hpp"

#include "utest.h"


CentralHeapType::HeapCreationParams params_central;
LocalHeapType::HeapCreationParams params_local;
constexpr std::size_t ARENA_CAPACITY=1024 * 1024 * 64;

std::atomic_bool allocator_initialized{false};

void* allocator_malloc(size_t _Size)
{
  if(!allocator_initialized)
  {
    const bool success=AllocatorType::get_instance().create(params_central, params_local, ARENA_CAPACITY);
    assert(success);
    allocator_initialized=true;
  }

  return AllocatorType::get_instance().allocate(_Size);
}

void allocator_free(void* _Block)
{
  assert(allocator_initialized);
  AllocatorType::get_instance().deallocate(_Block);
}

void* allocator_realloc(void* _Block, size_t _Size)
{
  assert(allocator_initialized);
  return AllocatorType::get_instance().reallocate(_Block, _Size);
}

UTEST_STATE();

size_t buginsight_internal_allocated_bytes(const char* p_function)
{
  char buffer[256]{};
  sprintf_s(buffer, "buginsight/get/mem/%s", p_function);

  size_t s{};
  DWORD ok=GetEnvironmentVariableA(buffer, (LPSTR)&s, sizeof(size_t));
  return s;
}

int main(int argc, const char* const argv[])
{
  return utest_main(argc, argv);
}
