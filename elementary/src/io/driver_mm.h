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


#ifndef DRIVER_MM_H
#define DRIVER_MM_H

#include "common/include.h"

template<class OBJECT>
class Driver_MM
{

  std::map<ID, OBJECT> content;
  
public:
  
  void init(ID id){
    
    if(content.find(id) != content.end()){
      // if already contains the key, error
      assert(false && "Driver_MM: duplicate key init'ed");
    }else{
      // else, insert using the default constructor
      content[id] = OBJECT();
    }
  }
  
  void set(ID id, const OBJECT & value){
    
    if(content.find(id) != content.end()){
      // if in hash
      content[id] = value;
    }else{
      assert(false && "Driver_MM: key not in hash");
    }
    
  }
  
  void get(ID id, OBJECT & value){
    
    if(content.find(id) != content.end()){
      // if in hash
      value = content[id];
    }else{
      assert(false && "Driver_MM: key not in hash");
    }
  }
  
  Driver_MM(){
    
  }
};

#endif // DRIVER_MM_H
