#include "gtest/gtest.h"

#include "common/jobconfig.h"
#include "fg/factorgraph.h"

TEST (FACTORGRAPH_TEST, LOAD){
  
  JobConfig jobconfig;
  
  jobconfig.filename_factors = "/tmp/deepdive/factors.tsv";
  jobconfig.filename_variables = "/tmp/deepdive/variables.tsv";
  jobconfig.filename_models = "/tmp/deepdive/models.tsv";
  jobconfig.workdir = "/tmp";
  jobconfig.buffer_size_in_byte = 40*1024L*1024;	//40MB
  jobconfig.frame_size_in_byte = 4096*1024;	//4MB
  jobconfig.nepoch = 30;
  
  FactorGraph<Driver_FILE, int> fg(jobconfig);
  
  fg.init();
  fg.sample();
}
