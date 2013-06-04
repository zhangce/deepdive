
#include "gtest/gtest.h"
#include "io/driver_mm.h"


TEST (Driver_MM, INIT){
  Driver_MM<int> mm;
  for(ID i=0;i<10000;i++){
    mm.init(i);
  }
  
  ASSERT_DEATH({for(ID i=0;i<10000;i++){mm.init(i);}}, "duplicate key init'ed");
}

TEST (Driver_MM, SETGET){
  Driver_MM<int> mm;
  for(ID i=0;i<10000;i+=2){
    mm.init(i);
  }
  for(ID i=1;i<10000;i+=2){
    mm.init(i);
  }

  for(ID i=0;i<10000;i++){
    mm.set(i, 3*i);
  }
  
  int value;
  for(ID i=0;i<10000;i++){
    mm.get(i, value);
    EXPECT_EQ(3*i, value);
  }
   
  ASSERT_DEATH({mm.get(-1, value);}, "key not in hash");
  ASSERT_DEATH({mm.set(-1, value);}, "key not in hash");
  ASSERT_DEATH({mm.get(20000, value);}, "key not in hash");
  ASSERT_DEATH({mm.set(20000, value);}, "key not in hash");
  
}