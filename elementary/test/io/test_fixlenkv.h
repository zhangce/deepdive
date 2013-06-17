
#include "gtest/gtest.h"
#include "gtest/gtest-spi.h"
#include "gtest/gtest-death-test.h"

#include "io/fixlenkv.h"

TEST (FIXLENKV_TEST, MANIPULATE_DOUBLE_ARRAY){
  FixLenKV<Driver_FILE, int> kv(40960, 4096, 4*sizeof(double), "/tmp/testfix");
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


TEST (FIXLENKV_TEST, MANIPULATE_DOUBLE){
  FixLenKV<Driver_FILE, int> kv(40960, 4096, sizeof(double), "/tmp/testfix");
  int page_id, offset_id;
  for(long i=0;i<10000000;i++){
    kv.add_record(i, sizeof(double), page_id, offset_id);
  }
  for(long i=0;i<10000000;i++){
    (*reinterpret_cast<double*>(kv.get_record(i))) = 3.14*i;
    kv.release_record(i);
  }
  for(long i=0;i<10000000;i++){
    EXPECT_EQ((*reinterpret_cast<double*>(kv.get_record(i))), 3.14*i);
  }
  for(long i=10000000-1;i>=0;i--){
    EXPECT_EQ((*reinterpret_cast<double*>(kv.get_record(i))), 3.14*i);
  }
}


TEST (FIXLENKV_TEST, LOAD_DOUBLE){
  FixLenKV<Driver_FILE, int> kv(40960, 4096, sizeof(double), "/tmp/testfix");
  int page_id, offset_id;
  for(long i=0;i<10000;i++){
    kv.add_record(i, sizeof(double), page_id, offset_id);
    EXPECT_EQ(i/kv.n_object_per_frame, page_id);
    EXPECT_EQ(i%kv.n_object_per_frame, offset_id);
    //std::cout << i << " " << page_id << " " << offset_id << std::endl;
  }
}

