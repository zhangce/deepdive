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

#include "common/include.h"

template<class OBJECT>
class Driver_FILE
{

public:
  
  int fd;
  std::string filename;
  OBJECT * alignedbuf;
  int ret;
  pthread_spinlock_t spinlock;
  
  std::map<ID, bool> objmask;	/*< We assume that we have hash-table for each object.*/
  
  void init(ID id){
    
    pthread_spin_lock(&spinlock);

    if(objmask.find(id) != objmask.end()){
      // if already contains the key, error
      assert(false && "Driver_FILE: duplicate key init'ed");
    }else{
      // else, insert using the default constructor
      objmask[id] = true;
    }
    
    ret = pwrite64(fd, alignedbuf, sizeof(OBJECT), id*sizeof(OBJECT));
    if(ret < 0){
      assert(false && "Driver_FILE: fail to write in init()");
    }
    
    pthread_spin_unlock(&spinlock);
    
  }
  
  void set(ID id, const OBJECT & value){
    
    pthread_spin_lock(&spinlock);
    
    if(objmask.find(id) != objmask.end()){
      // if in hash
      memcpy(alignedbuf, &value, sizeof(OBJECT));
      ret = pwrite64(fd, alignedbuf, sizeof(OBJECT), id*sizeof(OBJECT));
      if(ret < 0){
	assert(false && "Driver_FILE: fail to write in set()");
      }
    }else{
      assert(false && "Driver_FILE: key not in hash");
    }
    
    pthread_spin_unlock(&spinlock);
    
  }
  
  void get(ID id, OBJECT & value){
    
    pthread_spin_lock(&spinlock);
    
    if(objmask.find(id) != objmask.end()){
      // if in hash
      ret = pread64(fd, alignedbuf, sizeof(OBJECT), id*sizeof(OBJECT));
      if(ret < 0){
	assert(false && "Driver_FILE: fail to read in get()");
      }
      value = *alignedbuf;
    }else{
      assert(false && "Driver_FILE: key not in hash");
    }
    
    pthread_spin_unlock(&spinlock);
    
  }
  
  Driver_FILE(std::string _filename){
    
    assert(LOGICBLOCKSIZE <= sizeof(OBJECT) && "Driver_FILE: O_DIRECT cannot deal with OBJECT that is smaller than LOGICBLOCKSIZE");
    
    filename = _filename;
    fd = open64(filename.c_str(), O_RDWR | O_CREAT | O_DIRECT, (mode_t) 0600);
    
    if(fd <= 0){
      assert(false && "Driver_FILE: Fail to openfile");
    }
    
    ret = posix_memalign((void**)&alignedbuf, LOGICBLOCKSIZE, sizeof(OBJECT));
    if (ret != 0) {
      close(fd);
      assert(false && "Driver_FILE: Fail to align using posix_memalign");
    }
  }
  
  ~Driver_FILE(){
    free(alignedbuf);
    close(fd);
  }

};

#endif // DRIVER_FILE_H
