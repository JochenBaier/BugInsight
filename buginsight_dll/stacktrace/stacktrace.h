//This program is free software:you can redistribute it and /or modify it under the terms of the GNU
//General Public License as published by the Free Software Foundation, either version 3 of the License, or
//(at your option) any later version.
//
//This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
//even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//See the GNU General Public License for more details.
//
//You should have received a copy of the GNU General Public License along with this program.If not, see
//<https://www.gnu.org/licenses/>. 

#pragma once

#include "attributes.hpp"
#include "modules_helper.hpp"
#include "fmt/format.h"

struct callstack_t;
class xxhasher_t;

TM_ATTRIBUTE_NOINLINE int GetStackFrames(void** result, unsigned long* p_hash, int p_max_depth, int p_skip_count, bool p_check);
void InitializeSymbolizer();
void printf_call_stack_symbol_infos(const callstack_t& p_stack_frame, fmt::memory_buffer& p_callstack_string, const xxhasher_t& p_callstack_hash, const modules_static_runtime_t& p_modules_static_runtime);
TM_ATTRIBUTE_NOINLINE void save_current_callstack_to_string(fmt::memory_buffer& p_callstack_string, const int& p_skip_count);
