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
