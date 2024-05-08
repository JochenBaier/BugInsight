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

#include <iomanip>
#include <string>
#include <sstream>

namespace hex_dump
{
//initial from https://github.com/zmb3/hexdump/blob/master/Hexdump.hpp
template <unsigned RowSize, bool ShowAscii>
std::string to_string(const void* const p_dump, const size_t p_size)
{
  const uint8_t* const dump=(const uint8_t* const)p_dump;

  std::stringstream out;
  out.fill('0');
  for(size_t i=0; i < p_size; i+=RowSize)
  {
    out << (void*)&dump[i] << ": ";
    for(size_t j=0; j < RowSize; ++j)
    {
      if(i + j < p_size)
      {
        out << std::uppercase << std::hex << std::setw(2) << static_cast<int>(dump[i + j]) << " ";
      } else
      {
        out << "   ";
      }
    }

    out << " ";
    if(ShowAscii)
    {
      for(size_t j=0; j < RowSize; ++j)
      {
        if(i + j < p_size)
        {
          if(std::isprint(dump[i + j]))
          {
            out << static_cast<char>(dump[i + j]);
          } else
          {
            out << ".";
          }
        }
      }
    }
    out << '\n';
  }
  return out.str();
}

}
