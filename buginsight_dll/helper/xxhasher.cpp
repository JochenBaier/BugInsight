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
