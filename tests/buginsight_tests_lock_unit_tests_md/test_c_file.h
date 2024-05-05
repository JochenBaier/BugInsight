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

