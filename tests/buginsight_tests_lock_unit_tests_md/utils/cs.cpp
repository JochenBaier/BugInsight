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

#include "cs.hpp"


CRITICAL_SECTION* cs_maker_t::make_scoped()
{
  CRITICAL_SECTION* cs=new CRITICAL_SECTION;
  InitializeCriticalSection(cs);
  m_cs_list.push_back(cs);

  return cs;
}

cs_maker_t::~cs_maker_t()
{
  for(const auto& cs_p : m_cs_list)
  {
    DeleteCriticalSection(cs_p);
    delete cs_p;
  }
}
