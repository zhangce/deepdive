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

#ifndef DRIVER_FILE_H
#define DRIVER_FILE_H

#include <io/driver.h>

class Driver_FILE : public Driver
{

private:
  
  int fd;
  
  Frame empty_frame;
  
  int ret;
  
  pthread_spinlock_t spinlock;
  
  std::map<int, bool> objmask;	/*< We assume that we have hash-table for each frame.*/
  
public:
  virtual void add_frame(const int frameid);
  virtual void set_frame(const int frameid, const Frame& frame);
  virtual void get_frame(const int frameid, Frame& frame);
  Driver_FILE(const int & _framesize_in_byte, std::string _conn_string);
  virtual ~Driver_FILE();
};

#endif // DRIVER_FILE_H
