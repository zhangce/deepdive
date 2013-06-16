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

template<class DRIVER>
Scanner<DRIVER>::Scanner(int _framesize_in_byte, std::string _conn_string):
  frame(Frame(_framesize_in_byte)),
  driver(Driver_FILE(_framesize_in_byte, _conn_string)),
  current_frameid(0),
  framesize_in_byte(_framesize_in_byte)
{
  finalized = false;
  driver.add_frame(current_frameid);
  driver.get_frame(0, frame);
}

template<class DRIVER>
void Scanner<DRIVER>::finalize()
{
  if(!finalized){
    driver.set_frame(current_frameid, frame);
    finalized = true;
  }
  
}


template<class DRIVER>
void Scanner<DRIVER>::push_record(void* obj, int size)
{
  
  assert(finalized == false && "Scanner: cannot push new record after finalize!");
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

  //*frame.get_i_record_content(ret) = *obj;
  void * dest = memcpy(frame.get_i_record_content(ret), obj, size);
  assert(dest != NULL && "Scanner: Memcpy failed");
}

template<class DRIVER>
char * Scanner<DRIVER>::push_record(int size)
{
  assert(finalized == false && "Scanner: cannot push new record after finalize!");
  
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

  return frame.get_i_record_content(ret);
}

template<class DRIVER>
double Scanner<DRIVER>::scan(double (*func)(char*, void*), void * ppara)
{
  double sum = 0;
  
  this->finalize();
  std::cout << "  | Scaning " << (1+this->current_frameid) << " pages..." ;
  std::cout.flush();
  
  Frame frames[2] = {Frame(this->framesize_in_byte), Frame(this->framesize_in_byte)};
  
  int index_scan = 0;
  int index_load = 1;
  
  Timer totaltime;
  for(int frameid=0;frameid <= current_frameid;frameid++){
    Timer t;
    driver.get_frame(frameid, frames[index_scan]);
    for(int recid=0; recid<frames[index_scan].get_n_records(); recid++){
      sum += func(frames[index_scan].get_i_record_content(recid), ppara);
    }
    driver.set_frame(frameid, frames[index_scan]);
    double perepoch = t.elapsed();
    if(frameid == 0){
      std::cout << "[ETA=" << t.elapsed() * current_frameid << "]...";
      std::cout.flush();
    }
    
  }
  std::cout << "[TIME=" << totaltime.elapsed() << "]" << std::endl;
  
  return sum;
}

template<class DRIVER>
class Double_buffer_parameter{
public:
  DRIVER * driver;
  int frameid;
  int toprocess;
  Frame * frame;
  double sum;
  double (*func)(char*);
};

template<class DRIVER>
void * double_buffer_load(void * p){
  Double_buffer_parameter<DRIVER> * para = (Double_buffer_parameter<DRIVER> *) p;
  if(para->frameid - 2 != -1){
    para->driver->set_frame(para->frameid - 2, para->frame[para->toprocess]);
  }
  para->driver->get_frame(para->frameid, para->frame[para->toprocess]);
}

template<class DRIVER>
void * double_buffer_scan(void * p){
  Double_buffer_parameter<DRIVER> * para = (Double_buffer_parameter<DRIVER> *) p;
  para->sum = 0;
  for(int recid=0; recid<para->frame[para->toprocess].get_n_records(); recid++){
    para->sum += para->func(para->frame[para->toprocess].get_i_record_content(recid));
  }
}


/*
//TODO: Double buffering
template<class DRIVER>
double Scanner<DRIVER>::scan_prefetch(double (*func)(char*))
{
  double sum = 0;
  
  this->finalize();
  std::cout << "  | Scaning " << (1+this->current_frameid) << " pages..." << std::endl;
  Frame frames[2] = {Frame(this->framesize_in_byte), Frame(this->framesize_in_byte)};
  Double_buffer_parameter<DRIVER> paras[2];
  
  pthread_t f2_thread, f1_thread; 
  
  for(int i=0;i<2;i++){
    paras[i].driver = &this->driver;
    paras[i].func = func;
    paras[i].toprocess = i;
    paras[i].frame = frames;
  }
  
  int index_scan = 0;
  int index_load = 1;
  
  driver.get_frame(0, frames[index_scan]);
  
  for(int frameid=1;frameid <= current_frameid+1;frameid++){
    
    pthread_create(&f1_thread,NULL,double_buffer_scan<DRIVER>,&paras[index_scan]);
    
    if(frameid <= current_frameid){
      paras[index_load].frameid = frameid;
      pthread_create(&f2_thread,NULL,double_buffer_load<DRIVER>,&paras[index_load]);
    }
    
    if(frameid <= current_frameid){
      pthread_join(f1_thread,NULL);
      pthread_join(f2_thread,NULL);
    }else{
      pthread_join(f1_thread,NULL);
    }
    
    index_scan = 1 - index_scan;
    index_load = 1 - index_load; 
  }
  driver.set_frame(current_frameid, frames[index_load]);
  
  
  return sum;
}
*/

template<class DRIVER>
Scanner<DRIVER>::~Scanner()
{

}


// The following lines are used as workaround to seperated implementation and declration of template class
template class Scanner<Driver_FILE>;




