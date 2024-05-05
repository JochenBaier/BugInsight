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
