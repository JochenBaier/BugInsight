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

set (target buginsight_tests_leak_exe_md)

set(source_list 
main.cpp  
leaks.hpp
vld_bugs.hpp
)

add_executable(${target} ${source_list} )

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

endif()

target_compile_options(${target} PRIVATE /wd4100)
target_compile_options(${target} PRIVATE /wd4996)
target_compile_options(${target} PRIVATE /wd4267)
target_compile_options(${target} PRIVATE /wd4005)

set_property(TARGET ${target} PROPERTY VS_DEBUGGER_COMMAND "$<TARGET_FILE:buginsight>")
set_property(TARGET ${target} PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "-- $<TARGET_FILE:${target}>") 

