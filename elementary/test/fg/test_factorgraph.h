#include "gtest/gtest.h"

#include "common/jobconfig.h"
#include "fg/factorgraph.h"

TEST (FACTORGRAPH_TEST, LR_LEARNING){
  
  JobConfig jobconfig;
  
  // generate a simple LR
  std::ofstream fout_var("/tmp/test_factorgraph_vars.tsv");
  std::ofstream fout_fac("/tmp/test_factorgraph_facs.tsv");
  std::ofstream fout_mod("/tmp/test_factorgraph_mods.tsv");  
  for(long vid=0;vid<10000;vid++){
      fout_fac << vid << "\t" << 1 << "\t" << 1 << "\t" << 1 << std::endl;
      fout_var << vid << "\t" << "CES\t0\t1\t" << "1\t" << vid << "\t0\t0\t0\t" << (drand48()<0.8) << std::endl;
  }
  fout_mod << "1\t0.001\t0" << std::endl;
  fout_var.close();
  fout_fac.close();
  fout_mod.close();
  
  
  jobconfig.filename_factors = "/tmp/test_factorgraph_facs.tsv";
  jobconfig.filename_variables = "/tmp/test_factorgraph_vars.tsv";
  jobconfig.filename_models = "/tmp/test_factorgraph_mods.tsv";
  jobconfig.workdir = "/tmp";
  jobconfig.buffer_size_in_byte = 40*1024L*1024;	//40MB
  jobconfig.frame_size_in_byte = 4096*1024;	//4MB
  jobconfig.nepoch = 50;
  
  jobconfig.samplemode = SAMPLEMODE_ALLSAMPLE;
  jobconfig.burnin = 2;
  jobconfig.sgd_step_size = 0.01;
  
  jobconfig.is_log_system = true;
  
  FactorGraph<Driver_FILE, int> fg(jobconfig);
  
  fg.init();
  fg.sample();
  
  bool similar = fabs(ModelAccessor::model[1]-log(0.8/0.2))<0.1;
  EXPECT_EQ(similar, true);
  
}

TEST (FACTORGRAPH_TEST, LR_LEARNING_AND_INF){
  
  JobConfig jobconfig;
  
  // generate a simple LR
  std::ofstream fout_var("/tmp/test_factorgraph_vars.tsv");
  std::ofstream fout_fac("/tmp/test_factorgraph_facs.tsv");
  std::ofstream fout_mod("/tmp/test_factorgraph_mods.tsv");  
  for(long vid=0;vid<10000;vid++){
      fout_fac << vid << "\t" << 1 << "\t" << 1 << "\t" << 1 << std::endl;
      fout_var << vid << "\t" << "CES\t0\t1\t" << "1\t" << vid << "\t0\t0\t0\t" << (drand48()<0.8) << std::endl;
  }
  for(long vid=10000;vid<20000;vid++){
      fout_fac << vid << "\t" << 1 << "\t" << 1 << "\t" << 1 << std::endl;
      fout_var << vid << "\t" << "CQS\t0\t1\t" << "1\t" << vid << "\t0\t0\t0\t" << 0 << std::endl;
  }
  fout_mod << "1\t0.001\t0" << std::endl;
  fout_var.close();
  fout_fac.close();
  fout_mod.close();
  
  jobconfig.filename_factors = "/tmp/test_factorgraph_facs.tsv";
  jobconfig.filename_variables = "/tmp/test_factorgraph_vars.tsv";
  jobconfig.filename_models = "/tmp/test_factorgraph_mods.tsv";
  jobconfig.workdir = "/tmp";
  jobconfig.buffer_size_in_byte = 40*1024L*1024;	//40MB
  jobconfig.frame_size_in_byte = 4096*1024;	//4MB
  jobconfig.nepoch = 100;
  
  jobconfig.samplemode = SAMPLEMODE_LASTSAMPLE;
  jobconfig.burnin = 25;
  jobconfig.sgd_step_size = 0.01;
  
  jobconfig.is_log_system = true;
  
  FactorGraph<Driver_FILE, int> fg(jobconfig);
  
  fg.init();
  fg.sample();
  
  bool similar = fabs(ModelAccessor::model[1]-log(0.8/0.2))<0.1;
  EXPECT_EQ(similar, true);
  
  std::ifstream fin("/tmp/infrs_epoch_100.tsv");
  double sum = 0;
  double nvar = 0;
  long vid, lastsample;
  double expect = 0;
  while(fin >> vid >> lastsample >> expect){
    if(vid >= 10000){
      nvar ++;
      sum += expect;
    }
  }
  double avgexp = sum/nvar;
  //std::cout << avgexp << std::endl;
  
  similar = fabs(avgexp - 0.8)<0.1;
  EXPECT_EQ(similar, true);
  
  fin.close();
  
}

/*
TEST (FACTORGRAPH_TEST, LOAD){
  
  JobConfig jobconfig;
  
  jobconfig.filename_factors = "/tmp/deepdive/factors.tsv";
  jobconfig.filename_variables = "/tmp/deepdive/variables.tsv";
  jobconfig.filename_models = "/tmp/deepdive/models.tsv";
  jobconfig.workdir = "/tmp";
  jobconfig.buffer_size_in_byte = 40*1024L*1024;	//40MB
  jobconfig.frame_size_in_byte = 4096*1024;	//4MB
  jobconfig.nepoch = 5;
  
  jobconfig.samplemode = SAMPLEMODE_ALLSAMPLE;
  jobconfig.burnin = 2;
  jobconfig.sgd_step_size = 0.01;
  
  jobconfig.is_log_system = false;
  
  FactorGraph<Driver_FILE, int> fg(jobconfig);
  
  fg.init();
  fg.sample();
}
*/