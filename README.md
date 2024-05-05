A memory leak and deadlock finder for C, C++


## Introduction

**BugInsight** is a tool for finding memory leaks and potential deadlocks in C/C++ software for Microsoft Windows OS. 
BugInsight uses (improved) parts from  [Visual Leak Detector](https://github.com/KindDragon/vld) for memory leak finding. The deadlock detection algorithm is from the C++ library [Abseil](https://abseil.io/docs/cpp/guides/synchronization#deadlock-detection). 

Expect low overhead with I/O bound (e.g. TCP/IP) applications or Qt GUI applications.  A real world test with a TCP/IP application shows a slowdown of
10 %, with option "--callstacks=false" 5 %.

BugInsight can be used manually and automated in a CI system. BugInsight does not need to be linked and no rebuilding of the application is needed.

## Install

FIXME download and unzip

## Usage

`BugInsight.exe [Options] [Configuration] -- ["]application_to_test.exe["] [application_to_test options]`

Examples:

```bat
BugInsight.exe example_memory_leak.exe 
BugInsight.exe --on_error=fail -- example_deadlock.exe
BugInsight.exe --ignore_modules=libssl-3-x64.dll,libcrypto-3-x64.dll -- app.exe --app_option
```

Example output memory leak detection: [Example_Output](examples/example_memory_leak/Example_Output.md)   
Example output deadlock detection: [Example_Output](examples/example_deadlock/Example_Output.md)

<u>Usage in VS debugger</u> (the VS output window will display clickable call stacks):

1. Install VS Extension [Child Process Debugging Power Tool](https://devblogs.microsoft.com/devops/introducing-the-child-process-debugging-power-tool/)
2. VS project properties:  
   Debugging->Command: 'path to BugInsight.exe'  
   Debugging->Command Arguments: '-- $(TargetPath)'

## Application under test requirements

- C/C++ 64 bit application created with Visual Studio 2015 - 2022 (32 bit support is planned)
- Dynamic runtime: Multi-threaded [Debug] DLL (/MD[d]), expect false positive leak reports and missing deadlocks if static runtime is used
- For getting Calls Stacks: Debug or RelWithDebInfo configuration
- Regression testing (CI):  Release build recommended,  performance could be further improved with command line option '--callstacks=false'

## Features:

- Finding memory leaks and wrong memory API usage e.g. delete, delete[] mismatch
- Deadlock detection and detect wrong lock usage with CriticalSection and std::mutex, std::recursive_mutex
- Test applications (or parts of it) that cannot be tested on Linux with ThreadSanitizer and AddressSanitizer
- Test with the Windows operating system on which the program will later run
- Test applications that run too slowly with other test tools

- Lock watchdog: The time a lock is waiting for a lock or holding a lock  is monitored and a log or error is created if a maximum time is exceeded
- Periodic heap validation (_heapchk(), HeapValidate())
- Test C/C++ DLLs loaded in a Java program ( Java Native Interfaces)
- Test C/C++ DLLs loaded in a .Net program (P/Invoke)

## Monitored APIs

BugInsight can detect leaks or deadlocks from the following APIs: [Monitored APIs](Monitored_APIs.md). Please report missing APIs.  
BugInsight monitors all modules (Exe ,DLL) used by application under test <u>excluding</u> modules provided by Windows OS (DLLs with Copyright info: 'Microsoft Corporation', e.g. Kernel32.dll).  
BugInsight does <u>not yet</u> monitor handle leaks.

## Command Line Options, Runtime API

[Command Line Options](CommandLineOptions.md)  
[Runtime_API](Runtime_API.md)  

## Build BugInsight from source

[Build_from_source](Build_from_source.md)

## Alternatives (Windows OS)

- [Deleaker](https://www.deleaker.com) (Commercial)
- [Intel® Inspector](https://www.intel.com/content/www/us/en/developer/tools/oneapi/inspector.html#gs.8u1iuy) (Commercial, Free)
- [Memory Validator](https://www.softwareverify.com/product/memory-validator/) (Commercial)
- [Visual Leak Detector](https://github.com/KindDragon/vld) (Open Source, Inactive)
- Visual Studio build in tool for snapshot based leak finding
- more in [Qt Wiki](https://wiki.qt.io/Profiling_and_Memory_Checking_Tools)

## Used Third-party software

[Third-party software](Third_party_software.md)

## License

BugInsight is licensed under the terms of the Apache 2.0 license. See LICENSE for more information.

 