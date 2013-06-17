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


#include "driver_file.h"

void Driver_FILE::add_frame(const int frameid)
{
  assert(frameid >= 0 && "Driver_FILE: only support INTEGER-many frames for now. If you want larger storage, please increase the framesize instead.");
  
  pthread_spin_lock(&spinlock);

  if(objmask.find(frameid) != objmask.end()){
    // if already contains the key, error
    assert(false && "Driver_FILE: duplicate key init'ed");
  }else{
    // else, insert using the default constructor
    objmask[frameid] = true;
  }
    
  ret = pwrite64(fd, empty_frame.content, this->framesize_in_byte, 1L*frameid*this->framesize_in_byte);
  if(ret < 0){
    assert(false && "Driver_FILE: fail to write in init()");
  }
    
  pthread_spin_unlock(&spinlock);
}

void Driver_FILE::set_frame(const int frameid, const Frame& frame)
{
  pthread_spin_lock(&spinlock);
    
  if(objmask.find(frameid) != objmask.end()){
    // if in hash
    ret = pwrite64(fd, frame.content, this->framesize_in_byte, 1L*frameid*this->framesize_in_byte);
    if(ret < 0){
      assert(false && "Driver_FILE: fail to write in set()");
    }
  }else{
    assert(false && "Driver_FILE: key not in hash");
  }
    
  pthread_spin_unlock(&spinlock);
}

void Driver_FILE::get_frame(const int frameid, Frame& frame)
{
  pthread_spin_lock(&spinlock);
    
  if(objmask.find(frameid) != objmask.end()){
    // if in hash
    ret = pread64(fd, frame.content, this->framesize_in_byte, 1L*frameid*this->framesize_in_byte);
    if(ret < 0){
      assert(false && "Driver_FILE: fail to read in get()");
    }
  }else{
    assert(false && "Driver_FILE: key not in hash");
  }
    
  pthread_spin_unlock(&spinlock);
}

Driver_FILE::Driver_FILE(const int& _framesize_in_byte, std::string _conn_string): 
  Driver(_framesize_in_byte, _conn_string),
  empty_frame(Frame(_framesize_in_byte))
{
  
  pthread_spin_init(&this->spinlock, 0);
  
  
  assert(LOGICBLOCKSIZE <= this->framesize_in_byte && "Driver_FILE: O_DIRECT cannot deal with OBJECT that is smaller than LOGICBLOCKSIZE");  
  fd = open64(this->conn_string.c_str(), O_RDWR | O_CREAT | O_DIRECT, (mode_t) 0600);
    
  if(fd <= 0){
    assert(false && "Driver_FILE: Fail to openfile");
  }

}

Driver_FILE::~Driver_FILE()
{
  close(fd);
}

