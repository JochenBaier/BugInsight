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
