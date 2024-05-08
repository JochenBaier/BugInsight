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

set (target buginsight_dll)

cmake_minimum_required(VERSION 3.25)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_BUILD 0)
set(VERSION_REVISION 0)

configure_file(buginsight_rc.in buginsight.rc @ONLY)

set(source_list 
../shared/shared_settings.cpp
../shared/shared_settings.hpp
loader.cpp
loader.hpp
buginsight.rc
dll_main.cpp

graphcycles/graphcycles.cc
graphcycles/graphcycles.h
deadlock_monitor.cpp
deadlock_monitor.hpp
helper/low_level_alloc.cc
helper/low_level_alloc.h
dll_exports/deadlock_monitor_api.cpp
stacktrace/stacktrace.cc
stacktrace/stacktrace.h
thread_watchdog/thread_watchdog.cpp
thread_watchdog/thread_watchdog.hpp
helper/cs_mutex.cpp
helper/run_time_options.hpp
helper/cs_mutex.cpp
helper/cs_mutex.hpp
helper/xxhasher.cpp
helper/xxhasher.hpp
helper/hex_dump.hpp
helper/log.cpp
helper/log.hpp
helper/log_helper.cpp
helper/log_helper.hpp
helper/dbghelp_api.cpp
helper/dbghelp_api.hpp
iat/iat_patching.cpp
iat/iat_patching.hpp
iat/iat_helper.cpp
iat/iat_helper.hpp
iat/modules_helper.cpp
iat/modules_helper.hpp
iat/ldr_loader_lock.cpp
iat/ldr_loader_lock.hpp
iat/patched_functions.cpp
iat/patched_functions.hpp
helper/timer.hpp
api_monitors/critical_section_monitor.cpp
api_monitors/critical_section_monitor.hpp
thread_history/thread_history_helper.cpp
thread_history/thread_history_helper.hpp
helper/attributes.hpp
helper/one_shot_timer.cpp
helper/one_shot_timer.hpp
leak_check/leak_types.cpp
leak_check/leak_types.hpp
leak_check/memory_leak_map_with_callstack.cpp
leak_check/memory_leak_map_with_callstack.hpp
leak_check/malloc_monitor.cpp
leak_check/malloc_monitor.hpp
leak_check/_aligned_malloc_monitor.cpp
leak_check/_aligned_malloc_monitor.hpp
leak_check/strdup_monitor.cpp
leak_check/strdup_monitor.hpp
leak_check/memory_helper.cpp
leak_check/memory_helper.hpp
leak_check/heap_monitor.cpp
leak_check/heap_monitor.hpp
leak_check/getprocaddress_monitor.cpp
leak_check/getprocaddress_monitor.hpp
leak_check/local_alloc_monitor.cpp
leak_check/local_alloc_monitor.hpp
leak_check/globalalloc_monitor.cpp
leak_check/globalalloc_monitor.hpp
leak_check/getenvironmentvariable_monitor.cpp
leak_check/getenvironmentvariable_monitor.hpp
)

if (WIN32)
  set(source_list ${source_list}
          dll_exports/buginsight_exports.def
          api_monitors/mutex_monitor.cpp
          api_monitors/mutex_monitor.hpp
  )
endif()

add_library(${target} SHARED ${source_list})
set_property(TARGET ${target} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${target} PROPERTY CXX_STANDARD_REQUIRED on)

#message("CMAKE_GENERATOR_PLATFORM: ${CMAKE_GENERATOR_PLATFORM}")


if("${CMAKE_GENERATOR_PLATFORM}" MATCHES "Win32")
  message("32 bit configuration")
  set_target_properties(${target} PROPERTIES OUTPUT_NAME_DEBUG "BugInsightDllD32")
  set_target_properties(${target} PROPERTIES OUTPUT_NAME_RELEASE "BugInsightDll32")

else()
  message("64 bit configuration")
  set_target_properties(${target} PROPERTIES OUTPUT_NAME_DEBUG "BugInsightDllD64")
  set_target_properties(${target} PROPERTIES OUTPUT_NAME_RELEASE "BugInsightDll64")
  set_target_properties(${target} PROPERTIES OUTPUT_NAME_RELWITHDEBINFO "BugInsightDll64")
endif()

if(BUGINSIGHT_MAX_LOCKS_PER_THREAD)
  target_compile_definitions(${target} PRIVATE MAX_LOCKS_PER_THREAD=${BUGINSIGHT_MAX_LOCKS_PER_THREAD})
  else()
  target_compile_definitions(${target} PRIVATE MAX_LOCKS_PER_THREAD=20)
endif()


target_compile_definitions(${target} PRIVATE VERSION_MAJOR=${VERSION_MAJOR})
target_compile_definitions(${target} PRIVATE VERSION_MINOR=${VERSION_MINOR})
target_compile_definitions(${target} PRIVATE VERSION_BUILD=${VERSION_BUILD})
target_compile_definitions(${target} PRIVATE VERSION_REVISION=${VERSION_REVISION})

foreach(_source IN ITEMS ${source_list})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("${_group_path}" FILES "${_source}")
endforeach()

target_include_directories(${target}
 PRIVATE
   ${CMAKE_CURRENT_LIST_DIR}
    thread_history/
    stacktrace/
    thread_watchdog/
    graphcycles/
    output/
    helper/
    leak_check
    api_monitors/
    iat/
    ../shared
)

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
target_compile_options(${target} PRIVATE "/we4706") # assignment within conditional expression
target_compile_options(${target} PRIVATE "/we4129") # unrecognized character escape sequence

target_compile_options(${target} PRIVATE "/W4" )
target_compile_options(${target} PRIVATE "$<$<CONFIG:RELEASE>:/GL>" )
target_link_options(${target} PRIVATE "$<$<CONFIG:RELEASE>:/LTCG>" )

target_link_options(${target} PRIVATE "/entry:DllEntryPoint" )

target_link_libraries(${target} PRIVATE Dbghelp.lib psapi.lib Version.lib Advapi32.lib Crypt32.lib Shell32.lib)

find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

target_include_directories(${target} PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(${target} PRIVATE ${DETOURS_LIBRARY})

find_package(Boost REQUIRED COMPONENTS filesystem serialization)
target_link_libraries(${target} PRIVATE Boost::filesystem)
target_link_libraries(${target} PRIVATE Boost::serialization)

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${target} PRIVATE fmt::fmt)

find_package(xxhash CONFIG REQUIRED)
target_link_libraries(${target} PRIVATE xxHash::xxhash)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${target}  PRIVATE spdlog::spdlog_header_only)

