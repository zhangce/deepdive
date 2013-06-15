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


#ifndef BUFFER_H
#define BUFFER_H

#include "io/driver_file.h"

class BufferPagerHeader{
    
public:
    
    int page_id;
    
    int size_in_byte;
    
    bool is_dirty;
    
    Frame frame;
    
    BufferPageHeader(const int & _size_in_byte) :
        size_in_byte(_size_in_byte),
        frame(Frame(_size_in_byte))
    {
        page_id = -1;
    }
    
};

class PageHeader{
    
public:
    
    int page_id;
       
    int buf_id;
    
    BufferPageHeader * p_buffer_page;
        
    PageHeader(){
        p_buffer_page = NULL;
        buf_id = -1;
        page_id = -1;
    }
    
};


/**
 * \brief A PageBuffer is a buffer that requests frames from IO and maintain a page cache in it.
 *
 * \notes The current version is single-threaded.
 *
 */
template<class DRIVER, class PAGER>
class PageBuffer{
    
private:
    
    int next_empty_buffer;
    
public:
    
    size_t n_rio, n_wio;
    size_t n_miss;
    
    Driver_FILE driver;
    
    //PAGER pager;
    BufferPageHeader ** bufs;
    
    //PageHeader * page_table;
    std::vector<PageHeader> page_table;
    
    int current_size_page_table;
    
    const long buffer_size_in_byte;
    
    const int nbuffer;
    
    const int framesize_in_byte;
    
    PageBuffer(const int & _framesize_in_byte,
               const long & _buffer_size_in_byte);
    
    
    void add_page(const int & page_id);
    
    BufferPagerHeader * get_bufferpage_and_lock(const int & page_id);
    
    void release_lock(const int & page_id);
    
    size_t get_nio(){
        return n_rio + n_wio;
    }
};





#endif // BUFFER_H
