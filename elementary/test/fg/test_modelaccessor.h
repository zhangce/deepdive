#include "gtest/gtest.h"

#include "fg/modelaccessor.h"

TEST (MODELACCESSOR_TEST, LOAD){
  
  ModelAccessor ma;
  ma.load("/tmp/deepdive/models.tsv");

  EXPECT_EQ(ModelAccessor::model[0], 0.00001);
  EXPECT_EQ(ModelAccessor::model[1], 0.00002);
  EXPECT_EQ(ModelAccessor::model[2], 0.00003);
  
}