
#include "gtest/gtest.h"

#include "io/frame.h"


TEST (FRAME_TEST, VAR_SIZE_RECORDS){
  
  int framesizes[3] = {4096,	//4K
		       4194304,	//4M
		       1024*1024*1024 //1GB
  };
  
  for(int w=0;w<3;w++){
    int framesize = framesizes[w];
    //ustd::cout << framesizes[w] << std::endl;
    Frame f(framesize);
    EXPECT_EQ(f.get_n_records(), 0);
    EXPECT_EQ(f.get_c_meta_cursor_pos(), framesize - 4);
    EXPECT_EQ(f.get_c_content_cursor_pos(), 12);
    
    int sum = 0;
    int last = 0;
    int i;
    for(i=0;i<1024*1024*1024;i++){
      int size = (i+1)*sizeof(double);
      int rs = f.push_new_record(size);
      last = size;
      if(rs < 0){
	break;
      }
      sum += size;
      
    }
    //std::cout << "~~~" << i << std::endl;
    EXPECT_EQ(f.get_n_records(), i);
    EXPECT_LE(framesize-12-i*8, sum + last);
    
    for(i=0;i<f.get_n_records();i++){
      int ndouble = i+1;
      EXPECT_EQ(f.get_i_record_size(i), ndouble * sizeof(double));
      double * pd = reinterpret_cast<double*>(f.get_i_record_content(i));
      for(int j=0;j<ndouble;j++){
	pd[j] = (double)(3.14*i*j);
      }
    }
    
    for(i=0;i<f.get_n_records();i++){
      int ndouble = i+1;
      EXPECT_EQ(f.get_i_record_size(i), ndouble * sizeof(double));
      double * pd = reinterpret_cast<double*>(f.get_i_record_content(i));
      for(int j=0;j<ndouble;j++){
	EXPECT_EQ(pd[j], 3.14*i*j);
      }
    }

  } 
}


TEST (FRAME_TEST, DECONSTRUCTOR){
  Frame * pf = new Frame(4194304*100);
  ASSERT_NO_FATAL_FAILURE(delete pf);
}




TEST (FRAME_TEST, SAME_SIZE_RECORDS){
  
  int framesizes[3] = {4096,	//4K
		       4194304,	//4M
		       1024*1024*1024 //1GB
  };
  
  
  for(int w=0;w<3;w++){
    int framesize = framesizes[w];
    Frame f(framesize);
    EXPECT_EQ(f.get_n_records(), 0);
    EXPECT_EQ(f.get_c_meta_cursor_pos(), framesize - 4);
    EXPECT_EQ(f.get_c_content_cursor_pos(), 12);
    
    for(int i=0;i<1024*1024*1024;i++){
      int rs = f.push_new_record(sizeof(double));
      if(rs < 0){
	break;
      }
    }
    EXPECT_EQ(f.get_n_records(), (framesize - 12)/(8+sizeof(double))-1);

    for(int i=0;i<f.get_n_records();i++){
      *reinterpret_cast<double*>(f.get_i_record_content(i)) = 3.14*i;
    }
    
    for(int i=0;i<f.get_n_records();i++){
      EXPECT_EQ(*reinterpret_cast<double*>(f.get_i_record_content(i)), 3.14*i);
    }
  } 
}


