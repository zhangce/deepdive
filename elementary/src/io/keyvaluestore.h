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


#ifndef KEYVALUESTORE_H
#define KEYVALUESTORE_H

class KeyValueStore
{

public:
  
  KeyValueStore(){}
  
  virtual ~KeyValueStore(){}
  
  virtual void add_record(const long & object_id, const int & _object_size_in_byte, int & page_id, int & offset_id) = 0;
    
  virtual char * get_record(const long & object_id) = 0;
    
  virtual char * get_record(const int & page_id, const int & offset_id) = 0;
    
  virtual void release_record(const long & object_id) = 0;

  virtual void release_record(const int & page_id, const int & offset_id) = 0;

};

#endif // KEYVALUESTORE_H
