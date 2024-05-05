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

#include <cinttypes>

struct SynchLocksHeld;
struct user_options_t;

namespace thread_history_helper
{
bool update_lock_history_after_lock(const void* mu, const uint64_t& id, SynchLocksHeld& held_locks, const user_options_t& p_runtime_options);
void update_lock_history_in_lock(void* mu, const uint64_t& id, SynchLocksHeld& held_locks);
}
