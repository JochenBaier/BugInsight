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

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_compile_definitions(APP_NAME_PRETTY="BugInsight")
add_compile_definitions(APP_WNAME_PRETTY=L"BugInsight")


cmake_minimum_required(VERSION 3.25)
set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
message("CMAKE_TOOLCHAIN_FILE: ${CMAKE_TOOLCHAIN_FILE}")

set(VCPKG_TARGET_TRIPLET x64-windows-static-md)

project(buginsight-distribution)

set(CMAKE_CXX_STANDARD 17)

include(CTest)

add_subdirectory(buginsight "${CMAKE_CURRENT_BINARY_DIR}/buginsight")
add_subdirectory(buginsight_dll "${CMAKE_CURRENT_BINARY_DIR}/buginsight_dll")

add_subdirectory(tests/buginsight_tests_lock_unit_tests_md)
add_subdirectory(tests/buginsight_tests_leak_unit_tests_md)

add_subdirectory(tests/buginsight_tests_leak_exe_load_dll)
add_subdirectory(tests/buginsight_tests_leak_dll)
add_subdirectory(tests/buginsight_tests_leak_dll_mini)
add_subdirectory(tests/buginsight_tests_leak_dll_mini2)


add_subdirectory(tests/buginsight_tests_leak_exe_md)

add_subdirectory(examples/example_deadlock "${CMAKE_CURRENT_BINARY_DIR}/example_deadlock")
add_subdirectory(examples/example_memory_leak "${CMAKE_CURRENT_BINARY_DIR}/example_memory_leak")

include(ctests.cmake)

