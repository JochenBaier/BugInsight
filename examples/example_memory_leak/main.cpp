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

#include <cstdio>
#include <cstdlib>

#include <Windows.h>

int main()
{
  printf("start in console: 'BugInsight.exe example_memory_leak.exe'\n");

  void* p=malloc(100);
  char* s=_strdup("test string");
  void* h=HeapAlloc(GetProcessHeap(), 0, 42);

  printf("allocated malloc(): %p size: %zu\n", p, _msize(p));
  printf("allocated _strdup(): %p size: %zu\n", (void*)s, _msize(s));
  printf("allocated HeapAlloc(): %p size: %zu\n", (void*)h, HeapSize(GetProcessHeap(), 0, h));
  fflush(stdout);

  return 0;
}
