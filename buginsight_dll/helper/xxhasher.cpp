// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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

#include "xxhasher.hpp"

#include <cassert>

#include <boost/core/ignore_unused.hpp>

xxhasher_t::xxhasher_t()
{
  m_state64=XXH64_createState();
  assert(m_state64);

  XXH64_hash_t const seed{};
  XXH_errorcode res=XXH64_reset(m_state64, seed);
  boost::ignore_unused(res);
  assert(res != XXH_ERROR);
}

void xxhasher_t::add(const void* input, size_t len) const
{
  XXH_errorcode res=XXH64_update(m_state64, input, len);
  boost::ignore_unused(res);
  assert(res != XXH_ERROR);
}

uint64_t xxhasher_t::create_hash() const
{
  assert(m_state64);
  return XXH64_digest(m_state64);
}

xxhasher_t::~xxhasher_t()
{
  assert(m_state64);
  XXH64_freeState(m_state64);
}
