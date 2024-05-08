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

#Tests requires to be run in C:\dev\buginsight\build

set(target buginsight_tests_leak_unit_tests_md)

set(source_list 
../shared/test_helper.cpp
../shared/test_helper.hpp
main.cpp  
test_malloc_good.cpp
test_aligned_malloc_good.cpp
test_localalloc_good.cpp
test_globalalloc_good.cpp
test_heapalloc_good.cpp
allocator.hpp
utest.h
test_strdup_good.cpp
test_new_delete_good.cpp
test_winapi_good.cpp
test_loadlibrary_good.cpp
test_openssl_static_good.cpp

test_new_delete_bad.cpp
test_malloc_bad.cpp
test_localalloc_bad.cpp
test_globallalloc_bad.cpp
test_heapalloc_bad.cpp
test_aligned_malloc_bad.cpp
test_winapi_bad.cpp
test_strdup_bad.cpp
)


add_executable(${target} ${source_list} )

find_package(OpenSSL REQUIRED)

target_link_libraries(${target} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR} utils)

target_link_libraries(${target} PRIVATE Version.lib Advapi32.lib Crypt32.lib Shell32.lib Shlwapi.lib)


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
set_property(TARGET ${target} PROPERTY VS_DEBUGGER_COMMAND_ARGUMENTS "-- $<TARGET_FILE:${target}>") 


