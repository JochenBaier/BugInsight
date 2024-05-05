//Licensed to the Apache Software Foundation (ASF) under one
//or more contributor license agreements.  See the NOTICE file
//distributed with this work for additional information
//regarding copyright ownership.  The ASF licenses this file
//to you under the Apache License, Version 2.0 (the
//"License"); you may not use this file except in compliance
//with the License.  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//Unless required by applicable law or agreed to in writing,
//software distributed under the License is distributed on an
//"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
//KIND, either express or implied.  See the License for the
//specific language governing permissions and limitations
//under the License.  

#pragma once

#include "leak_types.hpp"
#include "attributes.hpp"

class memory_leak_repository_t;

namespace memory_helper
{
TM_ATTRIBUTE_NOINLINE bool log_or_abort_if_memory_adddress_is_unknown(memory_leak_repository_t& p_memory_leak_repository, void* _Block, api_family_t p_api_family, api_functions_t p_api_function);
}