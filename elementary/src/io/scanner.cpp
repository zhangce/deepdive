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


#include "scanner.h"

//template<class DRIVER>
Scanner::Scanner(int _framesize_in_byte, std::string _conn_string):
  frame(Frame(_framesize_in_byte)),
  driver(Driver_FILE(_framesize_in_byte, _conn_string)),
  current_frameid(0),
  framesize_in_byte(_framesize_in_byte)
{
  driver.add_frame(current_frameid);
  driver.get_frame(0, frame);
}

//template<class DRIVER>
void Scanner::finalize()
{
  driver.set_frame(current_frameid, frame);
}


//template<class DRIVER>
void Scanner::push_record(void* obj, int size)
{
  int ret = frame.push_new_record(size);
  
  if(ret < 0){	// the current page is full
    driver.set_frame(current_frameid, frame);
    current_frameid ++;
    driver.add_frame(current_frameid);
    driver.get_frame(current_frameid, frame);
  
    ret = frame.push_new_record(size);
    if(ret < 0){
      assert(false && "Scanner: Your object is larger than the page size... Please increse the size of you page");
    }
  }

  void * dest = memcpy(frame.get_i_record_content(ret), obj, size);
  assert(dest != NULL && "Scanner: Memcpy failed");
}

//template<class DRIVER>
double Scanner::scan()
{
  double sum = 0;
  
  this->finalize();
  std::cout << "  | Scaning " << (1+this->current_frameid) << " pages..." << std::endl;
  Frame frames[2] = {Frame(this->framesize_in_byte), Frame(this->framesize_in_byte)};

  int index_scan = 0;
  int index_load = 1;
  
  driver.get_frame(0, frames[index_scan]);
  
  for(int frameid=1;frameid <= current_frameid+1;frameid++){
    
    // scan
    for(int recid=0; recid<frames[index_scan].get_n_records(); recid++){
      sum += (*reinterpret_cast<double*>(frames[index_scan].get_i_record_content(recid)));
    }
    
    // load
    if(frameid <= current_frameid){
      driver.get_frame(frameid, frames[index_load]);
    }
    
    index_scan = 1 - index_scan;
    index_load = 1 - index_load; 
  }
  
  return sum;
}


//template<class DRIVER>
Scanner::~Scanner()
{

}

