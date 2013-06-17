#include "gtest/gtest.h"

#include "fg/modelaccessor.h"

TEST (MODELACCESSOR_TEST, LOAD){
  
  ModelAccessor ma;
  ma.load("/tmp/deepdive/models.tsv");

  EXPECT_EQ(ModelAccessor::model[1], 1);
  EXPECT_EQ(ModelAccessor::model[2], 0.001);
  EXPECT_EQ(ModelAccessor::model[3], 0.001);
  
}