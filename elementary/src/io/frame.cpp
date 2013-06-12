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


#include "frame.h"

Frame::Frame(const int & _size_in_byte)
{
  assert(_size_in_byte <= 1024*1024*1024 && _size_in_byte > 0 && "Frame: framesize over flow. We only support frame of 400M for now.");
  
  assert(LOGICBLOCKSIZE <= _size_in_byte && "Frame: Frame cannot deal with PAGESIZE that is smaller than LOGICBLOCKSIZE");  

  this->size_in_byte = _size_in_byte;
  
  int ret = posix_memalign((void**)&this->content, LOGICBLOCKSIZE, this->size_in_byte);
  
  if (ret != 0) {
    assert(false && "Frame: Fail to align using posix_memalign");
  }
  
  this->reset();
}

void Frame::reset()
{
  this->set_n_record(0);
  *reinterpret_cast<int*>(&this->content[4]) = 12;
  *reinterpret_cast<int*>(&this->content[8]) = this->size_in_byte - 4;
}

Frame::~Frame()
{
  delete [] this->content;
}

int Frame::get_n_records()
{
  return *reinterpret_cast<int*>(&this->content[0]);
}

void Frame::set_n_record(const int& nrecord)
{
  *reinterpret_cast<int*>(&this->content[0]) = nrecord;
}

int Frame::get_c_content_cursor_pos()
{
  return *reinterpret_cast<int*>(&this->content[4]);
}

int Frame::get_c_meta_cursor_pos()
{
  return *reinterpret_cast<int*>(&this->content[8]);
}

void Frame::set_c_content_cursor_pos(int cursor_pos)
{
  assert(cursor_pos < get_c_meta_cursor_pos() && "Frame: meta cursor_pos exceeds content cursor_pos!");
  *reinterpret_cast<int*>(&this->content[4]) = cursor_pos;
}

void Frame::set_c_meta_cursor_pos(int cursor_pos)
{
  assert(get_c_content_cursor_pos() < cursor_pos && "Frame: meta cursor_pos exceeds content cursor_pos!");
  *reinterpret_cast<int*>(&this->content[8]) = cursor_pos;
}

int Frame::get_i_record_pos(const int & i)
{
  assert(i < this->get_n_records() && "Frame: i^th record exceed nrecord!");
  assert(this->size_in_byte - 8 - 2*4*i > 0 && "Frame: i^th record's meta data exceeds 0!");
  return *reinterpret_cast<int*>(&this->content[this->size_in_byte - 8 - 2*4*i]);
}

int Frame::get_i_record_size(const int & i)
{
  assert(i < this->get_n_records() && "Frame: i^th record exceed nrecord!");
  assert(this->size_in_byte - 4 - 2*4*i > 0  && "Frame: i^th record's meta data exceeds 0!");
  return *reinterpret_cast<int*>(&this->content[this->size_in_byte - 4 - 2*4*i]);
}

void Frame::set_i_record_pos(const int& i, const int& pos)
{
  assert(i < this->get_n_records() && "Frame: i^th record exceed nrecord!");
  assert(this->size_in_byte - 8 - 2*4*i > 0 && "Frame: i^th record's meta data exceeds 0!");
  *reinterpret_cast<int*>(&this->content[this->size_in_byte - 8 - 2*4*i]) = pos;
}

void Frame::set_i_record_size(const int& i, const int& size)
{
  assert(i < this->get_n_records() && "Frame: i^th record exceed nrecord!");
  assert(this->size_in_byte - 4 - 2*4*i > 0  && "Frame: i^th record's meta data exceeds 0!");
  *reinterpret_cast<int*>(&this->content[this->size_in_byte - 4 - 2*4*i]) = size;
}


char* Frame::get_i_record_content(const int & i)
{
  return &this->content[this->get_i_record_pos(i)];
}

int Frame::push_new_record(const int & record_size_in_byte)
{
  

  assert(record_size_in_byte != 0 && "Frame: zero-size record!");
  
  int hypothesized_content_cursor = this->get_c_content_cursor_pos() + record_size_in_byte;
  int hypothesized_meta_cursor = this->get_c_meta_cursor_pos() - 2*4;
  
  if(hypothesized_content_cursor >= hypothesized_meta_cursor){
    return -1;
  }
  
  // get record id
  int i = this->get_n_records();
  this->set_n_record(i+1);
  
  // write down meta data first
  this->set_i_record_size(i, record_size_in_byte);
  this->set_i_record_pos(i, this->get_c_content_cursor_pos());
  this->set_c_meta_cursor_pos(this->get_c_meta_cursor_pos() - 2*4);
  
  // write down content data
  this->set_c_content_cursor_pos(this->get_c_content_cursor_pos() + record_size_in_byte);
  
  return i;
}


