
#include<iostream>

#include "common/jobconfig.h"
#include "fg/factorgraph.h"

int main(int argc, char **argv){
  
  std::cout << "Welcome to Elementary 0.3!" << std::endl;
  
  if(argc != 3){
    std::cout << "ERROR: Usage: ele <PATH TO ELEMENTARY PROJECT> <NITERATION>" << std::endl;
    return -1;
  }
  
  std::string filepath = std::string(argv[1]);
  int niter = atoi(argv[2]);
  
  JobConfig jobconfig;
  
  jobconfig.filename_factors = (filepath + "/factors.tsv").c_str();
  jobconfig.filename_variables = (filepath + "/variables.tsv").c_str();
  jobconfig.filename_models = (filepath + "/models.tsv").c_str();
  jobconfig.workdir = filepath.c_str();
  
  jobconfig.buffer_size_in_byte = 1L*1024L*1024L*1024;	//1GB
  jobconfig.frame_size_in_byte = 4096L*1024;	//4MB
  jobconfig.nepoch = niter;
  
  jobconfig.samplemode = SAMPLEMODE_ALLSAMPLE;
  jobconfig.burnin = niter/10;
  jobconfig.sgd_step_size = 0.01;
  
  jobconfig.is_log_system = true;
  
  FactorGraph<Driver_FILE, int> fg(jobconfig);
  
  fg.init();
  fg.sample();
    
  return 0;
}

