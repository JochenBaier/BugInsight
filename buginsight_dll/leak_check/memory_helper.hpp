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

#include "leak_types.hpp"
#include "attributes.hpp"

class memory_leak_repository_t;

namespace memory_helper
{
TM_ATTRIBUTE_NOINLINE bool log_or_abort_if_memory_adddress_is_unknown(memory_leak_repository_t& p_memory_leak_repository, void* _Block, api_family_t p_api_family, api_functions_t p_api_function);
}