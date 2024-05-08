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

cmake_minimum_required(VERSION 3.25)

set(target buginsight)

add_executable(${target} buginsight.rc main.cpp application_under_test.cpp application_under_test.hpp log.cpp log.hpp 
command_line_parser.cpp command_line_parser.hpp  ../shared/shared_settings.cpp ../shared/shared_settings.hpp 
options_parser.cpp options_parser.hpp ../shared/scope_exit.hpp)

set_target_properties(${target} PROPERTIES OUTPUT_NAME BugInsight)

target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../shared)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_BUILD 0)
set(VERSION_REVISION 0)

configure_file(buginsight_rc.in buginsight.rc @ONLY)

target_compile_definitions(${target} PRIVATE VERSION_MAJOR=${VERSION_MAJOR})
target_compile_definitions(${target} PRIVATE VERSION_MINOR=${VERSION_MINOR})
target_compile_definitions(${target} PRIVATE VERSION_BUILD=${VERSION_BUILD})
target_compile_definitions(${target} PRIVATE VERSION_REVISION=${VERSION_REVISION})

find_package(fmt CONFIG REQUIRED)
target_link_libraries(${target} PRIVATE fmt::fmt)

find_path(DETOURS_INCLUDE_DIRS "detours/detours.h")
find_library(DETOURS_LIBRARY detours REQUIRED)

target_include_directories(${target} PRIVATE ${DETOURS_INCLUDE_DIRS})
target_link_libraries(${target} PRIVATE ${DETOURS_LIBRARY})

find_package(Boost REQUIRED COMPONENTS program_options filesystem serialization)
target_link_libraries(${target} PRIVATE Boost::program_options)
target_link_libraries(${target} PRIVATE Boost::filesystem)
target_link_libraries(${target} PRIVATE Boost::serialization)

find_package(spdlog CONFIG REQUIRED)
target_link_libraries(${target}  PRIVATE spdlog::spdlog_header_only)

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
