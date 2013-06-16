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


#include "fixlenkv.h"

template<class DRIVER, class PAGER> 
FixLenKV<DRIVER, PAGER>::FixLenKV(const long int& _buffer_size_in_byte, const int& _frame_size_in_byte, const int& _object_size_in_byte, std::string _conn_string):
  buffer_size_in_byte(_buffer_size_in_byte),
  frame_size_in_byte(_frame_size_in_byte),
  object_size_in_byte(_object_size_in_byte),
  pagebuf(PageBuffer<DRIVER, PAGER>(frame_size_in_byte, buffer_size_in_byte, _conn_string))
{  
  
  // this is the most stupid way to calculate how many objects each page can hold.
  // TODO: change to some smarter way soon.
  Frame test_frame(frame_size_in_byte);
  while(test_frame.push_new_record(object_size_in_byte) >= 0);
  this->n_object_per_frame = test_frame.get_n_records();
  
}

template<class DRIVER, class PAGER>
void FixLenKV<DRIVER, PAGER>::add_record(const long int& object_id, const int & _object_size_in_byte, int& page_id, int& offset_id)
{
  assert(_object_size_in_byte == this->object_size_in_byte && "FixLenKV: object size is not consistent with this->object_size_in_byte");
  
  page_id = object_id / this->n_object_per_frame;
  offset_id = object_id % this->n_object_per_frame;
  
  if(!pagebuf.contain_page(page_id)){
    pagebuf.add_page(page_id);
  }
  BufferPageHeader * ph = pagebuf.get_bufferpage_and_lock(page_id);
  while(offset_id >= ph->frame.get_n_records()){
    ph->frame.push_new_record(object_size_in_byte);
  }
  pagebuf.release_lock(page_id);
}

template<class DRIVER, class PAGER>
char* FixLenKV<DRIVER, PAGER>::get_record(const int& page_id, const int& offset_id)
{
  BufferPageHeader * ph = pagebuf.get_bufferpage_and_lock(page_id);
  return ph->frame.get_i_record_content(offset_id);
}

template<class DRIVER, class PAGER>
void FixLenKV<DRIVER, PAGER>::release_record(const int& page_id, const int& offset_id)
{
  pagebuf.release_lock(page_id);
}

template<class DRIVER, class PAGER>
char* FixLenKV<DRIVER, PAGER>::get_record(const long int& object_id)
{
  return get_record(object_id / this->n_object_per_frame, object_id % this->n_object_per_frame);
}

template<class DRIVER, class PAGER>
void FixLenKV<DRIVER, PAGER>::release_record(const long int& object_id)
{
  release_record(object_id / this->n_object_per_frame, object_id % this->n_object_per_frame);
}




// The following lines are used as workaround to seperated implementation and declration of template class
template class FixLenKV<Driver_FILE, int>;

