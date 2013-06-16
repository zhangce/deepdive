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


#ifndef SCANNER_H
#define SCANNER_H

#include "io/driver_file.h"
#include "frame.h"
#include "common/include.h"

template<class DRIVER>
class Scanner
{
  
protected:
  DRIVER driver;
  //Driver_FILE driver;
  
  int framesize_in_byte;
  
  std::string conn_string;
  
  Frame frame;
  
  int current_frameid;
  
  bool finalized;
  
public:
  
  Scanner(int _framesize_in_byte, std::string _conn_string);
  
  virtual ~Scanner();
  
  char * push_record(int size);
  
  void push_record(void * obj, int size);

  void finalize();
  
  virtual double scan(double (*func)(char* wr_copy, char* rd_copy, void*, double* p_double1000buffer), void * ppara);

  //virtual double scan_prefetch(double (*func)(char*));
  
};

#endif // SCANNER_H
