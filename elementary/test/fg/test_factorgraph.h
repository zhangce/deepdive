#include "gtest/gtest.h"

#include "fg/factorgraph.h"

TEST (FACTORGRAPH_TEST, LOAD){
  
  FactorGraph<Driver_FILE, int> fg("/tmp/deepdive/models.tsv",
				   "/tmp/deepdive/variables.tsv",
				   "/tmp/deepdive/factors.tsv", 
				   40*1024L*1024, 4096*1024);
  
  fg.init();
}
