
#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"
#include "gtest/gtest-death-test.h"

#include "io/buffer.h"
#include "io/frame.h"


TEST (BUFFER_TEST, MANIPULATE_FRAME){
  
  for(int w=0;w<10;w++){
    
    PageBuffer<Driver_FILE, int> pagebuffer(4096, 40960, "/tmp/testbuffer");
    
    for(int i=0;i<100;i++){
      pagebuffer.add_page(i);
    }
  
    for(int i=0;i<100;i++){
      BufferPageHeader * ph = pagebuffer.get_bufferpage_and_lock(i);
      for(int j=0;j<10000;j++){
	int n = ph->frame.push_new_record(sizeof(double));
	if(n<0){
	  break;
	}
	*reinterpret_cast<double*>(ph->frame.get_i_record_content(n)) = 3.14*i*j;
      }    
      pagebuffer.release_lock(i);
    }
  
    for(int i=0;i<100;i++){
      BufferPageHeader * ph = pagebuffer.get_bufferpage_and_lock(i);
      for(int j=0;j<ph->frame.get_n_records();j++){
	EXPECT_EQ(*reinterpret_cast<double*>(ph->frame.get_i_record_content(j)), 3.14*i*j);
      }    
      pagebuffer.release_lock(i);
    }
  }

}




TEST (BUFFER_TEST, ADD_FRAME){
  PageBuffer<Driver_FILE, int> pagebuffer(4096, 40960, "/tmp/testbuffer");
  
  for(int i=0;i<100;i++){
    pagebuffer.add_page(i);
  }
 
  for(int i=0;i<10;i++){
    EXPECT_EQ(pagebuffer.bufs[i]->page_id, -1);
  }

  EXPECT_EQ(pagebuffer.page_table.size(), 100);

}
  
  
/*
TEST (BUFFER_TEST, TOO_SMALL_PAGE_LARGE_BUFFER){
  typedef PageBuffer<Driver_FILE, int> MYTYPE;
  ASSERT_DEATH({MYTYPE pagebuffer(512, 4096L*1024*1024*1024);}, "too large");
}
*/  
