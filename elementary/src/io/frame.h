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


#ifndef FRAME_H
#define FRAME_H

#include <common/include.h>

/**
 * \brief A frame, is an in-memory struture that contains one page stored in
 * secondary storages.
 * 
 * \note A frame is stored as follows
 * HEAD 
 *          NRECORD(INT)  CONTENT_CURSOR(INT)  META_CURSOR(INT) 
 *           CONTENT....
 * 
 * TAIL
 *          RECORD_CURSOR(INT) RECORD_SIZE(INT) 
 *          RECORD_CURSOR(INT) RECORD_SIZE(INT) 
 *          $
 * 
 * \note Frame::content is blocked-aligned, such that we can use it directly in DIRECTIO.
 * 
 * \note A frame itself does not build the association between PAGEID to content.
 * A frame is only the <b>content</b> of the page and a set of functions to manipulate
 * records stored in that page. 
 */
class Frame
{

private:
  
  int size_in_byte;
  
  void reset();
  
public:
  
  char * content;
  
  Frame(const int & _size_in_byte);
  
  virtual ~Frame();
  
  /**
   *
   * \note the first 0-4 elements in content is an integer for # records.
   */
  int get_n_records();
  
  /**
   *
   * \note the first 4-8 elements in content is an integer for # records.
   */
  int get_c_content_cursor_pos();
  
  /**
   *
   * \note the first 8-12 elements in content is an integer for # records.
   */
  int get_c_meta_cursor_pos();
  
  /**
   *
   * \note the first 4-8 elements in content is an integer for # records.
   */
  void set_c_content_cursor_pos(int cursor_pos);
  
  /**
   *
   * \note the first 8-12 elements in content is an integer for # records.
   */
  void set_c_meta_cursor_pos(int cursor_pos);
  
  /**
   * 
   * \note record grows from the end of the page.
   */
  int get_i_record_pos(const int &  i);
  
  int get_i_record_size(const int &  i);

  void set_i_record_pos(const int & i, const int & pos);
  
  void set_i_record_size(const int & i, const int & size);
  
  /**
   * 
   * \brief Get the i^th record's content as a pointer.
   * 
   * \note This function is not safe to eviction, which means that 
   *	the pointer returned might be changed if this frame is reloaded
   * 	as other page. The safety is maintained by the RandamAccessor
   * 	or Scanner, which takes care of eviction and function evalution
   * 	on the returned pointers.
   */
  char * get_i_record_content(const int &  i);
  
  /**
   * 
   * Try to add a new record with the given size into the frame.
   * 
   * \return -1 if failed; > 0 integer i, as i^th record.
   */
  int push_new_record(const int & record_size_in_byte); 
  
  void set_n_record(const int & nrecord);
  
};

#endif // FRAME_H
