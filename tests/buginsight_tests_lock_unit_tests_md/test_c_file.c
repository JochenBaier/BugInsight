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

#include "test_c_file.h"

#ifdef _WIN32
#include <Windows.h>
#endif


void test_c_file_thread_sanitzer_example()
{
  CRITICAL_SECTION mu1, mu2;
  InitializeCriticalSection(&mu1);
  InitializeCriticalSection(&mu2);

  // mu1 => mu2
  EnterCriticalSection(&mu1);
  EnterCriticalSection(&mu2);
  LeaveCriticalSection(&mu2);
  LeaveCriticalSection(&mu1);

  // mu2 => mu1
  EnterCriticalSection(&mu2);
  EnterCriticalSection(&mu1);  // <<<<<<< OOPS

  //REQUIRE(false);

  LeaveCriticalSection(&mu1);
  LeaveCriticalSection(&mu2);

  DeleteCriticalSection(&mu1);
  DeleteCriticalSection(&mu2);
}
