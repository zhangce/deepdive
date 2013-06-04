
#include "gtest/gtest.h"
#include "io/driver_file.h"


class page512{
public:
  int content[128];
};

TEST (Driver_FILE, INIT_PAGE512){
  Driver_FILE<page512> mm("/tmp/test1.txt");
  for(ID i=0;i<10000;i++){
    mm.init(i);
  }
  ASSERT_DEATH({for(ID i=0;i<10000;i++){mm.init(i);}}, "duplicate key init'ed");
}

TEST (Driver_FILE, SETGET){
  page512 page;
  Driver_FILE<page512> mm("/tmp/test2.txt");;
  for(ID i=0;i<10000;i+=2){
    mm.init(i);
  }
  for(ID i=1;i<10000;i+=2){
    mm.init(i);
  }

  for(ID i=0;i<10000;i++){
    page.content[0] = 3*i;
    page.content[127] = 4*i;
    mm.set(i, page);
  }
  
  page512 value;
  for(ID i=0;i<10000;i++){
    mm.get(i, value);
    EXPECT_EQ(3*i, value.content[0]);
    EXPECT_EQ(4*i, value.content[127]);
  }
   
  ASSERT_DEATH({mm.get(-1, value);}, "key not in hash");
  ASSERT_DEATH({mm.set(-1, value);}, "key not in hash");
  ASSERT_DEATH({mm.get(20000, value);}, "key not in hash");
  ASSERT_DEATH({mm.set(20000, value);}, "key not in hash");
  
}