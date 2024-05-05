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

#include <cstdlib>

#ifdef _WIN32
#include <Windows.h>
#include <vector>

#include <stdio.h>
#include <malloc.h>
#include <direct.h> 
#include <map>
#include <memory>
#include <map>
#include <string>

#define CAT_I(a, b) a ## b
#define CAT(a, b) CAT_I(a, b)
#define SCOPED_NAME(name) CAT(name, ALLOC_PREFIX_)


struct g_file_global_struct_t
{
  g_file_global_struct_t()
  {
#define ALLOC_PREFIX_ _in_file_global
#include "leaks.hpp"
  }
};

g_file_global_struct_t g_file_global;


struct g_file_global_static_struct_t
{
  g_file_global_static_struct_t()
  {
#define ALLOC_PREFIX_ _in_file_global_static
#include "leaks.hpp"
  }
};

g_file_global_static_struct_t g_file_global_static;


#define ALLOC_PREFIX_ _in_main_cpp
#include "leaks.hpp"

void atexit_function()
{
#define ALLOC_PREFIX_ _in_atexit
#include "leaks.hpp"
}

int onexit_function(void)
{
#define ALLOC_PREFIX_ _in__onexit
#include "leaks.hpp"
  return 0;
}


#include "vld_bugs.hpp"

extern "C" void c_function_call()
{
  void* p=malloc(50);
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
  switch(ul_reason_for_call)
  {
  case DLL_PROCESS_ATTACH:
  {
    atexit(atexit_function);
    _onexit(onexit_function);

#define ALLOC_PREFIX_ _in_process_attach
#include "leaks.hpp"
  } break;

  case DLL_PROCESS_DETACH:
  {
    return TRUE;

  } break;
  default: break;
  }

  return TRUE;
}
#endif
