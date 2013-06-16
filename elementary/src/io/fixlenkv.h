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


#ifndef FIXLENKV_H
#define FIXLENKV_H

#include "io/buffer.h"
#include "io/keyvaluestore.h"

template<class DRIVER, class PAGER> 
class FixLenKV : public KeyValueStore{

protected:
  
  const long buffer_size_in_byte;
    
  const int frame_size_in_byte;
    
  const int object_size_in_byte;
  
  PageBuffer<DRIVER, PAGER> pagebuf;
  
public:
  
  int n_object_per_frame;
    
  FixLenKV(const long & _buffer_size_in_byte,
             const int  & _frame_size_in_byte,
             const int  & _object_size_in_byte,
	     std::string _conn_string);
    
  void add_record(const long & object_id, const int & _object_size_in_byte, int & page_id, int & offset_id);
    
  char * get_record(const long & object_id);
    
  char * get_record(const int & page_id, const int & offset_id);
    
  void release_record(const long & object_id);

  void release_record(const int & page_id, const int & offset_id);
    
};

#endif // FIXLENKV_H
