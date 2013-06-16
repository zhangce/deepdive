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


#include "buffer.h"

template<class DRIVER, class PAGER>
PageBuffer<DRIVER, PAGER>::PageBuffer(const int & _framesize_in_byte,
                                      const long & _buffer_size_in_byte,
				      std::string _conn_string):
    framesize_in_byte(_framesize_in_byte),
    buffer_size_in_byte(_buffer_size_in_byte),
    nbuffer(_buffer_size_in_byte/_framesize_in_byte),
    driver(Driver_FILE(_framesize_in_byte, _conn_string.c_str())){
    
        assert(1L* nbuffer * framesize_in_byte == buffer_size_in_byte && "PageBuffer: framesize_in_byte too small and _buffer_size_in_byte too large.");
        
        this->bufs = new BufferPageHeader*[nbuffer];
        for(int i=0;i<nbuffer;i++){
            bufs[i] = new BufferPageHeader(framesize_in_byte);
        }
        
        n_rio = 0;
        n_wio = 0;
        n_miss = 0;
        
        next_empty_buffer = 0;
}

template<class DRIVER, class PAGER>
PageBuffer<DRIVER, PAGER>::~PageBuffer(){
  delete [] this->bufs;
}

template<class DRIVER, class PAGER>
void PageBuffer<DRIVER, PAGER>::add_page(const int & page_id){
    
    if(page_id >= page_table.size()){
        while(page_id >= page_table.size()){
            page_table.push_back(PageHeader());
        }
    }
    
    driver.add_frame(page_id);
    
    page_table[page_id].page_id = page_id;
    
}


template<class DRIVER, class PAGER>
BufferPageHeader * PageBuffer<DRIVER, PAGER>::get_bufferpage_and_lock(const int & page_id){
    
    assert(page_id < page_table.size() && "PageBuffer: page_id not in page_table.");
    assert(page_table[page_id].page_id == page_id);
    
    if(page_table[page_id].buf_id == -1){
        //evict one
        //random evict
        int buf_to_load = -1;
        if(next_empty_buffer < nbuffer){
            buf_to_load = next_empty_buffer;
            next_empty_buffer ++;
        }else{
            buf_to_load = rand() % nbuffer;
            // flush
            driver.set_frame(bufs[buf_to_load]->page_id, bufs[buf_to_load]->frame);
            //
            page_table[bufs[buf_to_load]->page_id].buf_id = -1;
            page_table[bufs[buf_to_load]->page_id].p_buffer_page = NULL;
        }
        assert(bufs[buf_to_load]->page_id != page_id);
        
        //load
        bufs[buf_to_load]->page_id = page_id;
        bufs[buf_to_load]->is_dirty = false;
        driver.get_frame(bufs[buf_to_load]->page_id, bufs[buf_to_load]->frame);
        
        //update page table
        page_table[page_id].p_buffer_page = bufs[buf_to_load];
        page_table[page_id].buf_id = buf_to_load;
    }
    return page_table[page_id].p_buffer_page;
}

template<class DRIVER, class PAGER>
void PageBuffer<DRIVER, PAGER>::release_lock(const int & page_id){
}




// The following lines are used as workaround to seperated implementation and declration of template class
template class PageBuffer<Driver_FILE, int>;


