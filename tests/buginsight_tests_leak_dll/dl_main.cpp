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
