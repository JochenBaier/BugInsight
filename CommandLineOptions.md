```
BugInsight.exe --help
Usage: BugInsight.exe [Options] [Configuration] -- ["]application_to_test.exe["] [application_to_test options]:

Options:
  -v [ --version ]        print version string
  -h [ --help ]           produce help message

Configuration:
  --on_error arg          fail|fail_on_exit|continue, default: fail_on_exit
                          'fail': Abort immediately with exit code '66'
                          'fail_on_exit': Finish with exit code '66' on process
                          end
                          'continue': application_to_test.exe will end with
                          standard error code

  --ignore_modules arg    Module list, comma separated:
                          a.dll[{ml}],b.dll[{ml}],c.exe[{ml}],...
                          Module name can contain wildcards: Qt6*.dll
                          Ignore module memory leaks only: a.dll{m}
                          Ignore module dead lock check only: b.dll{l}
                          Disable memory leak check for all modules: *{m}
                          Disable deadlock check for all modules: *{l}

  --callstacks arg        true|false, default: true
                          'true': Call Stacks for locks and memory allocations
                          will be collected.
                          'false': No Call Stacks will be collected. Use for CI
                          tests to improve performance
                          if there are currently no issues open

  --log_to arg            List of these values, comma separated:
                          stdout|pathtofile.txt
                          default: stdout
                          e.g.: stdout,"C:/temp/log file.txt"

  --log_to_debug_out arg  enabled|disabled|debugger, default: debugger
                          'enabled':  log to debugger output
                          'disabled': no log to debugger output
                          'debugger': log to debugger output if debugger
                          attached

  --log_level arg         trace|debug|info|warn|error|fatal|off, default: info

  --exit_code arg         1..126, exit code on error, default: 66
  --start_detached arg    true|false, default: false
                          'true': application_to_test.exe will be start non
                          blocking. BugInsight.exe stops immediately.
                          'false': application_to_test.exe will be start
                          blocking. BugInsight.exe runs until
                          application_to_test.exe stops.
```

