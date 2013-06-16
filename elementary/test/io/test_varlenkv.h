
#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"
#include "gtest/gtest-death-test.h"

#include "io/varlenkv.h"


TEST (VARLENKV_TEST, MANIPULATE_DOUBLE_ARRAY){
  VarLenKV<Driver_FILE, int> kv(40960, 4096, "/tmp/testvar");
  int page_id, offset_id;
  for(long i=0;i<10000000;i++){
    kv.add_record(i, 4*sizeof(double), page_id, offset_id);
  }
  for(long i=0;i<10000000;i++){
    for(int j=0;j<4;j++){
      (reinterpret_cast<double*>(kv.get_record(i)))[j] = 3.14*i*j;
    }
    kv.release_record(i);
  }
  for(long i=0;i<10000000;i++){
    for(int j=0;j<4;j++){
      EXPECT_EQ((reinterpret_cast<double*>(kv.get_record(i)))[j], 3.14*i*j);
      (reinterpret_cast<double*>(kv.get_record(i)))[j] = 3.14*i*j + 1;
    }
  }
  for(long i=10000000-1;i>=0;i--){
    for(int j=0;j<4;j++){
      EXPECT_EQ((reinterpret_cast<double*>(kv.get_record(i)))[j], 3.14*i*j+1);
    }
  }
}

TEST (VARLENKV_TEST, MANIPULATE_DOUBLE){
  VarLenKV<Driver_FILE, int> kv(40960, 4096, "/tmp/testvar");
  int page_id, offset_id;
  for(long i=0;i<10000000;i++){
    kv.add_record(i, sizeof(double), page_id, offset_id);
    
    int d1, d2;
    kv.to_record_id(i, d1, d2);
    EXPECT_EQ(page_id, d1);
    EXPECT_EQ(offset_id, d2);
  }
  
  for(long i=0;i<10000000;i++){
    (*reinterpret_cast<double*>(kv.get_record(i))) = 3.14*i;
  }
  
  for(long i=0;i<10000000;i++){
    EXPECT_EQ((*reinterpret_cast<double*>(kv.get_record(i))), 3.14*i);
  }
  
  for(long i=10000000-1;i>=0;i--){
    EXPECT_EQ((*reinterpret_cast<double*>(kv.get_record(i))), 3.14*i);
  }
  
}

TEST (VARLENKV_TEST, LOAD_DOUBLE){
  FixLenKV<Driver_FILE, int> kv_gt(40960, 4096, sizeof(double), "/tmp/testfix");
  VarLenKV<Driver_FILE, int> kv(40960, 4096, "/tmp/testvar");
  int page_id, offset_id;
  int page_id2, offset_id2;
  for(long i=0;i<10000;i++){
    kv_gt.add_record(i, sizeof(double), page_id2, offset_id2);
    kv.add_record(i, sizeof(double), page_id, offset_id);
    EXPECT_EQ(page_id, page_id2);
    EXPECT_EQ(offset_id, offset_id2);
  }
}


