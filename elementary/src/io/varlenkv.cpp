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


#include "varlenkv.h"

template<class DRIVER, class PAGER>
void VarLenKV<DRIVER, PAGER>::release_record(const int& page_id, const int& offset_id)
{
  this->pagebuf.release_lock(page_id);
}

template<class DRIVER, class PAGER>
char* VarLenKV<DRIVER, PAGER>::get_record(const int& page_id, const int& offset_id)
{
  BufferPageHeader * ph = this->pagebuf.get_bufferpage_and_lock(page_id);
  return ph->frame.get_i_record_content(offset_id);
}

template<class DRIVER, class PAGER>
void VarLenKV<DRIVER, PAGER>::release_record(const long & object_id)
{
  int page_id, offset_id;
  this->to_record_id(object_id, page_id, offset_id);
  this->release_record(page_id, offset_id);
}

template<class DRIVER, class PAGER>
char* VarLenKV<DRIVER, PAGER>::get_record(const long & object_id)
{
  int page_id, offset_id;
  this->to_record_id(object_id, page_id, offset_id);
  return this->get_record(page_id, offset_id);
}

template<class DRIVER, class PAGER>
void VarLenKV<DRIVER, PAGER>::to_record_id(const long & object_id, int& page_id, int& offset_id)
{
  
  int * rids = (reinterpret_cast<int*>(this->kv_oid2rid.get_record(object_id)));
  
  page_id = rids[0];
  offset_id = rids[1];
    
  this->kv_oid2rid.release_record(object_id);
}

template<class DRIVER, class PAGER>
void VarLenKV<DRIVER, PAGER>::add_record(const long & object_id, const int& _object_size_in_byte, int& page_id, int& offset_id)
{
  // try to add to current tail
  page_id = this->current_tail_page_id;

  offset_id = this->pagebuf.get_bufferpage_and_lock(page_id)->frame.push_new_record(_object_size_in_byte);
  this->pagebuf.release_lock(page_id);
  
  if(offset_id < 0){
    // this means that the previous page is full
    this->current_tail_page_id ++;
    page_id = this->current_tail_page_id;
    
    this->pagebuf.add_page(this->current_tail_page_id);
    offset_id = this->pagebuf.get_bufferpage_and_lock(page_id)->frame.push_new_record(_object_size_in_byte);
    if(offset_id < 0){
      assert(false && "VarLenKV: Object size is larger than page size.");
    }
    this->pagebuf.release_lock(page_id);
  }
  
  // record object_id -> page_id, offset_id mapping
  this->kv_oid2rid.add_record(object_id, 2*sizeof(int), dummy1, dummy2);
  int * rids = (reinterpret_cast<int*>(this->kv_oid2rid.get_record(object_id)));
  rids[0] = page_id;
  rids[1] = offset_id;
  this->kv_oid2rid.release_record(object_id);
}

template<class DRIVER, class PAGER>
VarLenKV<DRIVER, PAGER>::VarLenKV(const long int& _buffer_size_in_byte, const int& _frame_size_in_byte, std::string _conn_string):
  buffer_size_in_byte(_buffer_size_in_byte),
  frame_size_in_byte(_frame_size_in_byte),
  kv_oid2rid(FixLenKV<DRIVER, PAGER>(buffer_size_in_byte/2, frame_size_in_byte, 2*sizeof(int), _conn_string + "_OID2RID")),
  pagebuf(PageBuffer<DRIVER, PAGER>(frame_size_in_byte, buffer_size_in_byte/2, _conn_string))
{
  this->current_tail_page_id = 0;
  this->pagebuf.add_page(0);
}

template<class DRIVER, class PAGER>
VarLenKV<DRIVER, PAGER>::~VarLenKV()
{

}


// The following lines are used as workaround to seperated implementation and declration of template class
template class VarLenKV<Driver_FILE, int>;

