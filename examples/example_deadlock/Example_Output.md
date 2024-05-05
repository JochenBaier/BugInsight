## example deadlock - output

```
BugInsight.exe --on_error=fail -- example_deadlock.exe
[BugInsight] [info] BugInsight Application under test: 'example_deadlock.exe'
[BugInsight] [info] Loading 'BugInsight' into application under test.
[BugInsight] [info] BugInsight loaded (version: '0.1.0.0')

[Used BugInsight options]
  on error: fail
  ignore modules memory leak: ''
  ignore modules deadlock: ''
  collect Call Stacks: true
  start detached: false
  log to: stdout
  log level: info
  thread_watchdog: true
  thread_watchdog_max_duration_sec: 30
  log to debug out: in debugger
  exit_code: 66


start in console: 'BugInsight.exe --on_error=fail -- example_deadlock.exe'
Philosopher 0 picked up left fork
Philosopher 2 picked up left fork
Philosopher 3 picked up left fork
Philosopher 4 picked up left fork
Philosopher 1 picked up left fork



==========================================================================================================
[BugInsight] [error] Potential deadlock at lock: 0x7ff70886b4c8, thread ID: 25268
Hint: Go through all Call Stacks and check which locks are called in inconsistent order

Lock Call Stack:
C:\dev\buginsight\buginsight_dll\deadlock_monitor.cpp(155,0): deadlock_monitor_t::dlc_deadlock_check_before_lock (BugInsightDll64.dll)
C:\dev\buginsight\buginsight_dll\api_monitors\critical_section_monitor.cpp(67,0): buginsight_enter_critical_section (BugInsightDll64.dll)
C:\dev\buginsight\examples\example_deadlock\main.cpp(38,0): philosopher_thread_function (example_deadlock.exe)
[...]
0x7fff7daf9333 (ucrtbase.dll) (no symbol info)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)

A cycle in the historical lock ordering graph has been observed:


Thread ID: 25268
Holding lock: 0x7ff70886b4a0
Wait for lock: 0x7ff70886b4c8
Call Stack:
C:\dev\buginsight\buginsight_dll\api_monitors\critical_section_monitor.cpp(67,0): buginsight_enter_critical_section (BugInsightDll64.dll)
C:\dev\buginsight\examples\example_deadlock\main.cpp(38,0): philosopher_thread_function (example_deadlock.exe)
[...]
0x7fff7daf9333 (ucrtbase.dll) (no symbol info)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)


Thread ID: 14872
Holding lock: 0x7ff70886b4c8
Wait for lock: 0x7ff70886b4f0
Call Stack:
C:\dev\buginsight\buginsight_dll\api_monitors\critical_section_monitor.cpp(67,0): buginsight_enter_critical_section (BugInsightDll64.dll)
C:\dev\buginsight\examples\example_deadlock\main.cpp(38,0): philosopher_thread_function (example_deadlock.exe)
[...]
0x7fff7daf9333 (ucrtbase.dll) (no symbol info)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)


Thread ID: 25552
Holding lock: 0x7ff70886b4f0
Wait for lock: 0x7ff70886b450
Call Stack:
C:\dev\buginsight\buginsight_dll\api_monitors\critical_section_monitor.cpp(67,0): buginsight_enter_critical_section (BugInsightDll64.dll)
C:\dev\buginsight\examples\example_deadlock\main.cpp(38,0): philosopher_thread_function (example_deadlock.exe)
[...]
0x7fff7daf9333 (ucrtbase.dll) (no symbol info)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)


Thread ID: 23944
Holding lock: 0x7ff70886b450
Wait for lock: 0x7ff70886b478
Call Stack:
C:\dev\buginsight\buginsight_dll\api_monitors\critical_section_monitor.cpp(67,0): buginsight_enter_critical_section (BugInsightDll64.dll)
C:\dev\buginsight\examples\example_deadlock\main.cpp(38,0): philosopher_thread_function (example_deadlock.exe)
[...]
0x7fff7daf9333 (ucrtbase.dll) (no symbol info)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)


Thread ID: 4012
Holding lock: 0x7ff70886b478
Wait for lock: 0x7ff70886b4a0
Call Stack:
C:\dev\buginsight\buginsight_dll\api_monitors\critical_section_monitor.cpp(67,0): buginsight_enter_critical_section (BugInsightDll64.dll)
C:\dev\buginsight\examples\example_deadlock\main.cpp(38,0): philosopher_thread_function (example_deadlock.exe)
[...]
0x7fff7daf9333 (ucrtbase.dll) (no symbol info)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)
[BugInsight] [error] Process will be terminated with exit code '66' due to potential deadlock. Use command line option '--on_error=continue (or fail_on_exit)' to continue on error

[BugInsight] [info] Application under test finished with exit code '66'
```

