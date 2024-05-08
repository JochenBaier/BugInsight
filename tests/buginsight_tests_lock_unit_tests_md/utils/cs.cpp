// This is an open source non-commercial project. Dear PVS-Studio, please check it.

// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

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
