## Runtime API

BugInsight (miss)use the WinAPI function 'GetEnvironmentVariableA' as runtime API. BugInsight monitors calls to GetEnvironmentVariableA.
The env variable is virtual and the system env variables are not touched. With this trick no linking to BugInsight or using LoadLibrary()/GetProcAddress() is needed.

Ignore next lock call in source:

`GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);`

Ignore next unlock call in source:

`GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);`

Example:

```c++
//ignore next EnterCriticalSection call:
GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);
EnterCriticalSection(CriticalSectionA);

//ignore next LeaveCriticalSection call:
GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);
LeaveCriticalSection(CriticalSectionA);

//ignore lock, unlock with std::lock_guard 
GetEnvironmentVariableA("buginsight/set/lock++", nullptr, 0);
GetEnvironmentVariableA("buginsight/set/unlock++", nullptr, 0);
const std::lock_guard lock_guard(m_mutex);

```