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
#include <thread>
#include <Windows.h>

//start in console: 'BugInsight.exe --on_error=fail -- example_deadlock.exe'

const int NUM_PHILOSOPHERS=5;
CRITICAL_SECTION forks[NUM_PHILOSOPHERS];

void philosopher_thread_function(int id, CRITICAL_SECTION& left_fork, CRITICAL_SECTION& right_fork)
{
  while(true)
  {
    EnterCriticalSection(&left_fork);
    EnterCriticalSection(&left_fork);
    std::cout << "Philosopher " << id << " picked up left fork" << '\n';

    // Introduce a deliberate delay to increase the chances of deadlock
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    EnterCriticalSection(&right_fork);
    std::cout << "Philosopher " << id << " picked up right fork" << '\n';

    // Eat or think (not implemented in this example)

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    std::cout << "Philosopher " << id << " released both forks" << '\n';
    LeaveCriticalSection(&left_fork);
    LeaveCriticalSection(&right_fork);
  }
}

int main()
{
  printf("start in console: 'BugInsight.exe --on_error=fail -- example_deadlock.exe'\n");

  // Initialize critical sections
  for(auto& fork : forks)
  {
    InitializeCriticalSection(&fork);
  }

  std::thread philosophers[NUM_PHILOSOPHERS];

  // Start philosophers with their respective left and right forks
  for(int i=0; i < NUM_PHILOSOPHERS; ++i)
  {
    philosophers[i]=std::thread(philosopher_thread_function, i, std::ref(forks[i]), std::ref(forks[(i + 1) % NUM_PHILOSOPHERS]));
  }

  // Wait for the philosophers to finish (will not happen in this case due to deadlock)
  for(auto& philosopher : philosophers)
  {
    philosopher.join();
  }

  // Clean up critical sections
  for(auto& fork : forks)
  {
    DeleteCriticalSection(&fork);
  }

  return 0;
}
