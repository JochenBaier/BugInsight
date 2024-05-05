#Licensed to the Apache Software Foundation (ASF) under one
#or more contributor license agreements.  See the NOTICE file
#distributed with this work for additional information
#regarding copyright ownership.  The ASF licenses this file
#to you under the Apache License, Version 2.0 (the
#"License"); you may not use this file except in compliance
#with the License.  You may obtain a copy of the License at
#
#  http://www.apache.org/licenses/LICENSE-2.0
#
#Unless required by applicable law or agreed to in writing,
#software distributed under the License is distributed on an
#"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
#KIND, either express or implied.  See the License for the
#specific language governing permissions and limitations
#under the License.  

set(target buginsight_tests_lock_unit_tests_md)

set(source_list 
../shared/test_helper.cpp
../shared/test_helper.hpp
main.cpp  
utils/signal.cpp
utils/signal.hpp
utils/cs.cpp
utils/cs.hpp
utils/random_numbers.cpp
utils/random_numbers.hpp
test_critical_section_lock_order_inversion.cpp
test_critical_section_lock_order_ok_or_disabled.cpp
test_thread_watchdog_bad.cpp
test_thread_watchdog_good.cpp
)

if (WIN32)
set(source_list ${source_list}
        test_std_mutex_good.cpp
        test_std_mutex_bad.cpp
        test_std_recursive_mutex_good.cpp
        test_std_recursive_mutex_bad.cpp
        test_c_file.c
        test_c_file.h
    )
endif()


add_executable(${target} ${source_list} )

target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR} utils)


if (WIN32)
  #flags for standard conformance
  target_compile_options(${target} PRIVATE "/permissive-")
  target_compile_options(${target} PRIVATE "/volatile:iso")
  target_compile_options(${target} PRIVATE "/Zc:inline")
  target_compile_options(${target} PRIVATE "/Zc:preprocessor")
  target_compile_options(${target} PRIVATE "/Zc:lambda")
  target_compile_options(${target} PRIVATE "/Zc:__cplusplus")
  target_compile_options(${target} PRIVATE "/Zc:externConstexpr")
  target_compile_options(${target} PRIVATE "/Zc:throwingNew")
  target_compile_options(${target} PRIVATE "/EHsc")
   
  #this warnings as errors
  target_compile_options(${target} PRIVATE "/we4062") # enumerator in switch not handled
  target_compile_options(${target} PRIVATE "/we4296") # '>=': expression is always true
  target_compile_options(${target} PRIVATE "/we4715") # not all control paths return a value
  target_compile_options(${target} PRIVATE "/we4553") # '==': operator has no effect; did you intend '='?
  target_compile_options(${target} PRIVATE "/we4552") # '>=': operator has no effect; expected operator with side-effect
  target_compile_options(${target} PRIVATE "/we4456") # declaration of 'xxx' hides previous local declaration
  target_compile_options(${target} PRIVATE "/we4239") # nonstandard extension used
  target_compile_options(${target} PRIVATE "/we4129") # unrecognized character escape sequence

  target_compile_options(${target} PRIVATE "/W3" )

endif()

target_compile_options(${target} PRIVATE /wd4100)
target_compile_options(${target} PRIVATE /wd4996)
target_compile_options(${target} PRIVATE /wd4267)



set_property(TARGET ${target} PROPERTY VS_DEBUGGER_COMMAND "$<TARGET_FILE:buginsight>")

set_property(TARGET ${target} PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "--thread_watchdog_max_duration_sec=5 -- $<TARGET_FILE:${target}>") 
