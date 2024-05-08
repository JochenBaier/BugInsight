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

#pragma  once

#include "pthread.h"

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
// used by C++ source code
#endif


typedef pthread_mutex_t CRITICAL_SECTION;

void buginsight_initialize_critical_section(void* lpCriticalSection);
void buginsight_delete_critical_section(void* lpCriticalSection);
void buginsight_enter_critical_section(void* lpCriticalSection);
int buginsight_tryenter_critical_section(void* lpCriticalSection);
void buginsight_leave_critical_section(void* lpCriticalSection);

#ifdef __cplusplus
}
#endif
