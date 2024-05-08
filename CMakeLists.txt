#This program is free software:you can redistribute it and /or modify it under the terms of the GNU
#General Public License as published by the Free Software Foundation, either version 3 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
#even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#See the GNU General Public License for more details.
#
#You should have received a copy of the GNU General Public License along with this program.If not, see
#<https://www.gnu.org/licenses/>. 

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

