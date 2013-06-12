/*
    Copyright 2013 <copyright holder> <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#ifndef DRIVER_H
#define DRIVER_H

#include "common/include.h"
#include "frame.h"

/**
 * \brief Abstract interface to I/O Drivers.
 * 
 * \note Secondary storages store a sequence of Frame of same size, each of which has an integer ID.
 * 
 */
class Driver
{

protected:
  std::string conn_string;
  const int framesize_in_byte;
  
public:
  Driver(const int & _framesize_in_byte, std::string _conn_string):
    framesize_in_byte(_framesize_in_byte)
  {
    conn_string = _conn_string;
  }
  
  virtual ~Driver(){
    
  }
  
  virtual void get_frame(const int frameid, Frame & frame) = 0;
  
  virtual void set_frame(const int frameid, const Frame & frame) = 0;
  
  virtual void add_frame(const int frameid) = 0;
};
#endif // DRIVER_H
