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

#include "xxhash.h"

class xxhasher_t
{
public:
  xxhasher_t();
  void add(const void* input, size_t len) const;
  [[nodiscard]] uint64_t create_hash() const;
  ~xxhasher_t();

private:
  XXH64_state_t* m_state64{nullptr};
};
