## example memory leak - output

```
BugInsight.exe example_memory_leak.exe
[BugInsight] [info] BugInsight Application under test: 'example_memory_leak.exe'
[BugInsight] [info] Loading 'BugInsight' into application under test.
[BugInsight] [info] BugInsight loaded (version: '0.1.0.0')

[Used BugInsight options]
  on error: fail on exit
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


start in console: 'BugInsight.exe example_memory_leak.exe'
allocated malloc(): 0000019350639F70 size: 100
allocated _strdup(): 0000019350657740 size: 12
allocated HeapAlloc(): 0000019350659820 size: 42
The application under test is currently being shut down...
Analysing '3' leaks...


Memory leak report:

Function                      |     Calls|     Bytes allocated
----------------------------------------------------------------
malloc                        |         1|                 100
HeapAlloc                     |         1|                  42
_strdup                       |         1|                  12


Memory leak callstacks:


[Function: 'malloc', Hit count: 1, Sum bytes allocated: 100]

Call Stack:
C:\dev\buginsight\examples\example_memory_leak\main.cpp(28,0): main (example_memory_leak.exe)
vcstartup\src\startup\exe_common.inl(288,0): __scrt_common_main_seh (example_memory_leak.exe)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)


[Function: 'HeapAlloc', Hit count: 1, Sum bytes allocated: 42]

Call Stack:
C:\dev\buginsight\examples\example_memory_leak\main.cpp(31,0): main (example_memory_leak.exe)
vcstartup\src\startup\exe_common.inl(288,0): __scrt_common_main_seh (example_memory_leak.exe)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)


[Function: '_strdup', Hit count: 1, Sum bytes allocated: 12]

Call Stack:
C:\dev\buginsight\examples\example_memory_leak\main.cpp(28,0): main (example_memory_leak.exe)
vcstartup\src\startup\exe_common.inl(288,0): __scrt_common_main_seh (example_memory_leak.exe)
0x7fff7f15257d (KERNEL32.DLL) (no symbol info)
0x7fff8076aa48 (ntdll.dll) (no symbol info)


==================
BugInsight summary:
==================
[Ok     ] Monitored API calls:        3
[Ok     ] Potential deadlocks found:  No
[Error  ] Memory leaks found:         Yes
[Ok     ] Errors found:               No

[BugInsight] [error] Terminate process with exit code '66' due to errors. Use command line option '--on_error=continue' to ignore errors.
[BugInsight] [info] Application under test finished with exit code '66'
```

