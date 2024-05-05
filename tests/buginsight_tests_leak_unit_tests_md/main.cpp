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
