/*
    Copyright 2013 <copyright holder> <email>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/


#include "factorgraph.h"
#include "sampler/discretegibbssampler.h"

template<class DRIVER, class PAGER>
double process_variable_init(char * wr_copy, char * rd_copy, void * ppara, double* p_double100buffer){
  
  FactorAccessor<DRIVER, PAGER> * factors = reinterpret_cast<FactorAccessor<DRIVER, PAGER>*>(ppara);
  Variable * pvar = reinterpret_cast<Variable*>(wr_copy);
  
  long fid;
  
  for(long i_factor=0;i_factor<pvar->nfactor;i_factor++){
    fid = *pvar->get_i_fid(i_factor);
    FactorFactory::init_factor(factors->get_record(fid), i_factor, pvar);
    factors->release_record(fid);
  }
  
  return 0;
}

template<class DRIVER, class PAGER>
double process_variable_dump(char * wr_copy, char * rd_copy, void * ppara, double* p_double1000buffer){
  FILE * file = (FILE *) ppara;
  Variable * pvar = reinterpret_cast<Variable*>(wr_copy);
  if(pvar->dtype == 'C'){
    fprintf (file, "%ld\t%d\t%f\n", pvar->vid, pvar->cvalue, pvar->sample_sum/pvar->nsample);
  }
  if(pvar->dtype == 'R'){
    fprintf (file, "%ld\t%f\t%f\n", pvar->vid, pvar->rvalue, pvar->sample_sum/pvar->nsample);
  }
  if(pvar->dtype == 'M'){
    assert(false);
  }
}

template<class DRIVER, class PAGER>
double process_variable_sample(char * wr_copy, char * rd_copy, void * ppara, double* p_double1000buffer){
  
  FactorAccessor<DRIVER, PAGER> * factors = reinterpret_cast<FactorAccessor<DRIVER, PAGER>*>(ppara);
  Variable * pvar = reinterpret_cast<Variable*>(wr_copy);
  Variable * pvar_ori = reinterpret_cast<Variable*>(rd_copy);
  
  long fid;
  int changed;
  bool is_generated = false;
  
  // first try to call vg function (the first factor)
  if(pvar->stype == 'G'){
    fid = *pvar->get_i_fid(0);
    is_generated = FactorFactory::variable_generate(factors->get_record(fid), 0, pvar, pvar_ori);
    assert(is_generated==true && "FactorGraph: The first factor should be VG function is stype=G");
  }
  
  // else sample!
  if(pvar->stype == 'S'){
 

    if(pvar->dtype == 'C' && (pvar->upper <= 999 && pvar->lower >= 0)){
      // for discrete variables with small range, just enumerate all possibilities
      changed = DiscreteGibbsSampler<DRIVER, PAGER>::sample(factors, pvar, pvar_ori, p_double1000buffer, ModelAccessor::is_log_system);
    }else if(pvar->dtype == 'C' || pvar->dtype == 'R'){
      // for discrete variables with large range, or continuous variables, the 
      // last thing we could do is MH with uniform proposal
    }else if(pvar->dtype == 'M'){
      // for now, we do not support matrix
      assert(false);
    }
    
  }
  
  // update
  if(pvar->vtype == 'Q'){
    // for query variables, update factor content (inference)
    for(long i_factor=0;i_factor<pvar->nfactor;i_factor++){
      fid = *pvar->get_i_fid(i_factor);      
      FactorFactory::update_factor(factors->get_record(fid), i_factor, pvar, pvar_ori);
      factors->release_record(fid);
    }
  }else if(pvar->vtype == 'E'){
    // for evidence variables, update model (learning)
    for(long i_factor=0;i_factor<pvar->nfactor;i_factor++){
      fid = *pvar->get_i_fid(i_factor);      
      FactorFactory::update_model(factors->get_record(fid), i_factor, pvar, pvar_ori);
      factors->release_record(fid);
    }
    if(pvar->dtype == 'C'){
      pvar->cvalue = pvar_ori->cvalue;
    }else if(pvar->dtype == 'R'){
      pvar->rvalue = pvar_ori->rvalue;
    }else if(pvar->dtype == 'M'){
      assert(false);
    }else{
      assert(false);
    }
  }else{
    assert(false);
  }
  
  //tally
  if(ModelAccessor::start_tally){
    if(pvar->dtype == 'C'){
      pvar->sample_sum += pvar->cvalue;
    }else if(pvar->dtype == 'R'){
      pvar->sample_sum += pvar->rvalue;
    }else if(pvar->dtype == 'M'){
      assert(false);
    }else{
      assert(false);
    }
    pvar->nsample++;
  }
  
  return changed;
}

template<class DRIVER, class PAGER>
void FactorGraph<DRIVER, PAGER>::init()
{
  std::cout << "  | INITIALIZATION EPOCH...";
  this->variables.scan(&process_variable_init<DRIVER, PAGER>, &this->factors);
  std::cout << std::endl;
}


template<class DRIVER, class PAGER>
void FactorGraph<DRIVER, PAGER>::sample()
{
  for(int i=0;i<this->jobconfig.nepoch;i++){
    
    std::cout << "  | Epoch " << (i+1) << " / " << this->jobconfig.nepoch << "...";
    if(i >= jobconfig.burnin){
      ModelAccessor::start_tally = true;
    }
    
    this->variables.scan(&process_variable_sample<DRIVER, PAGER>, &this->factors);
    
    if(jobconfig.samplemode == SAMPLEMODE_ALLSAMPLE){
      if(i < jobconfig.burnin){
	std::cout << "    * Skip dumping because still in burnin phase" << std::endl;
      }else{
	std::string dump_filename = jobconfig.workdir + "/infrs_epoch_" + SSTR(i+1) + ".tsv";
	std::cout << "    * Dumping epoch " << (i+1) << " to " << dump_filename << "...";
	FILE * pFile = fopen (dump_filename.c_str(),"w");
	this->variables.scan(&process_variable_dump<DRIVER, PAGER>, pFile);
	fclose (pFile);
      }
    }
    std::cout << std::endl;
  }
  if(jobconfig.samplemode == SAMPLEMODE_LASTSAMPLE){
    std::string dump_filename = jobconfig.workdir + "/infrs_epoch_" + SSTR(jobconfig.nepoch) + ".tsv";
    std::cout << "  | Dumping last epoch to " << dump_filename << "...";
    FILE * pFile = fopen (dump_filename.c_str(),"w");
    this->variables.scan(&process_variable_dump<DRIVER, PAGER>, pFile);
    fclose (pFile);
    std::cout << std::endl;
  }
  
  // finally, we dump the model
  std::string dump_filename = jobconfig.workdir + "/infrs_models.tsv";
  std::cout << "  | Dumping models to " << dump_filename << "...";
  FILE * pFile = fopen (dump_filename.c_str(),"w");
  
  for(int i=1;i<ModelAccessor::model.size();i++){
    fprintf(pFile, "%d\t%f\n", i, ModelAccessor::model[i]);
  }
  fclose (pFile);
  std::cout << std::endl;
}

template<class DRIVER, class PAGER>
FactorGraph<DRIVER, PAGER>::FactorGraph(JobConfig _jobconfig):
  jobconfig(_jobconfig),
  variables(VariableScanner<DRIVER>(_jobconfig.frame_size_in_byte, _jobconfig.workdir + "/vars")),
  factors(FactorAccessor<DRIVER, PAGER>(_jobconfig.buffer_size_in_byte, _jobconfig.frame_size_in_byte, _jobconfig.workdir + "/facs"))
{  
  Timer t1;
  std::cout << "  | Loading variables..." << std::endl;
  std::cout << "    # var = " << this->variables.load(jobconfig.filename_variables) << std::endl;
  std::cout << "    # [TIME=" << t1.elapsed() << "]" << std::endl;
  
  Timer t2;
  std::cout << "  | Loading factors..." << std::endl;
  std::cout << "    # fac = " << this->factors.load(jobconfig.filename_factors) << std::endl;
  std::cout << "    # [TIME=" << t2.elapsed() << "]" << std::endl;
  
  Timer t3;
  std::cout << "  | Loading models..." << std::endl;
  std::cout << "    # mod = " << ModelAccessor::load(jobconfig.filename_models) << std::endl;
  std::cout << "    # [TIME=" << t3.elapsed() << "]" << std::endl;
  
  ModelAccessor::sgd_step_size = jobconfig.sgd_step_size;
  ModelAccessor::is_log_system = jobconfig.is_log_system;
  std::cout << "  | Setting parameters..." << std::endl;
  std::cout << "    # sgd_step_size = " << ModelAccessor::sgd_step_size << std::endl;
  std::cout << "    # is_log_system = " << ModelAccessor::is_log_system << std::endl;
  
  std::cout << std::endl;
}

template<class DRIVER, class PAGER>
FactorGraph<DRIVER, PAGER>::~FactorGraph()
{

}

// The following lines are used as workaround to seperated implementation and declration of template class
template class FactorGraph<Driver_FILE, int>;


