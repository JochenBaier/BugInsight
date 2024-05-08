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

#ifdef _WIN32
#include <Windows.h>
#endif

#pragma once
#ifdef __cplusplus
extern "C" {
#endif

  void test_c_file_thread_sanitzer_example();

  inline void test_in_header()
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

#ifdef __cplusplus
}
#endif

