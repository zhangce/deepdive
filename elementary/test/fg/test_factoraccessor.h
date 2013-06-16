#include "gtest/gtest.h"

#include "fg/factoraccessor.h"

TEST (FACTORACCESSOR_TEST, LOAD){
  
  FactorAccessor<Driver_FILE, int> fa(40960, 4096, "/tmp/testfactoraccessor");
  long nfactor = fa.load("/tmp/deepdive/factors.tsv");
  EXPECT_EQ(nfactor, 37081);
  
  for(int i=0;i<nfactor;i++){
    char * pstate = fa.get_record(i);
    bool a = (reinterpret_cast<int*>(pstate))[0] == 0 || (reinterpret_cast<int*>(pstate))[0] == 1 || (reinterpret_cast<int*>(pstate))[0]==2;
    EXPECT_EQ(a, true);
  }
  
}