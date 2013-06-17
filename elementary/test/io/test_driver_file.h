#include "gtest/gtest.h"

#include "io/driver_file.h"
#include <io/frame.h>


TEST (DRIVER_FILE_TEST, ADD_FRAME){
  
  Driver_FILE driver(4*1024*1024, "/tmp/mytest");
  Frame f(4*1024*1024);
  for(int i=0;i<100;i++){
    driver.add_frame(i);

    driver.get_frame(i,f);
    EXPECT_EQ(f.get_n_records(), 0);
    EXPECT_EQ(f.get_c_content_cursor_pos(), 12);
    EXPECT_EQ(f.get_c_meta_cursor_pos(), 4*1024*1024 - 4);    
  }
  
  for(int i=0;i<100;i++){
    driver.get_frame(i,f);
    for(int j=0;j<100;j++){
      f.push_new_record(sizeof(double));
    }
    for(int j=0;j<100;j++){
      *reinterpret_cast<double*>(f.get_i_record_content(j)) = 3.14*i*j;
    }
    driver.set_frame(i,f);
  }
  
  for(int i=0;i<100;i++){
    driver.get_frame(i,f);
    for(int j=0;j<100;j++){
      EXPECT_FLOAT_EQ(*reinterpret_cast<double*>(f.get_i_record_content(j)), 3.14*i*j);
    }
  }
  
  
  
}
