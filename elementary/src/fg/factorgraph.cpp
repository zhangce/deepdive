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
double process_variable_sample(char * wr_copy, char * rd_copy, void * ppara, double* p_double1000buffer){
  
  FactorAccessor<DRIVER, PAGER> * factors = reinterpret_cast<FactorAccessor<DRIVER, PAGER>*>(ppara);
  Variable * pvar = reinterpret_cast<Variable*>(wr_copy);
  Variable * pvar_ori = reinterpret_cast<Variable*>(rd_copy);
  
  long fid;
  
  if(pvar->dtype == 'C' && (pvar->upper <= 999 && pvar->lower >= 0)){
  
    for(int newvalue=pvar->lower;newvalue<=pvar->upper;newvalue++){
      p_double1000buffer[newvalue] = 1.0;
    }
    
    for(long i_factor=0;i_factor<pvar->nfactor;i_factor++){
      fid = *pvar->get_i_fid(i_factor);   
      char * fstate = factors->get_record(fid);
      // propose
      for(int newvalue=pvar->lower;newvalue<=pvar->upper;newvalue++){
	
	*pvar->get_i_value(0) = newvalue;
	
	double potential = FactorFactory::potential_factor(fstate, i_factor, pvar, pvar_ori);
	
	p_double1000buffer[newvalue] *= potential;
      }
      
      factors->release_record(fid);
    }
    
    double sum = 0;
    for(int newvalue=pvar->lower;newvalue<=pvar->upper;newvalue++){
      sum += p_double1000buffer[newvalue];
    }
    
    double r = drand48();
    double accum = 0;
    double ratio = 0;
    int topick = -1;
    for(int newvalue=pvar->lower;newvalue<=pvar->upper;newvalue++){
      accum += p_double1000buffer[newvalue];
      ratio = accum / sum;
      if(r <= ratio){
	topick = newvalue;
	break;
      }
    }
    assert(topick != -1);
    
    *pvar->get_i_value(0) = topick;
    
    // update
    for(long i_factor=0;i_factor<pvar->nfactor;i_factor++){
      fid = *pvar->get_i_fid(i_factor);      
      FactorFactory::update_factor(factors->get_record(fid), i_factor, pvar, pvar_ori);
      factors->release_record(fid);
    }
    
    if(topick != *pvar_ori->get_i_value(0)){
      return 1;
    }else{
      return 0;
    }
    
    
  }else{
    assert(false);
  }
    
}


template<class DRIVER, class PAGER>
void FactorGraph<DRIVER, PAGER>::init()
{
  this->variables.scan(&process_variable_init<DRIVER, PAGER>, &this->factors);
}


template<class DRIVER, class PAGER>
void FactorGraph<DRIVER, PAGER>::sample()
{
  for(int i=0;i<this->jobconfig.nepoch;i++){
    this->variables.scan(&process_variable_sample<DRIVER, PAGER>, &this->factors);
  }
}

template<class DRIVER, class PAGER>
FactorGraph<DRIVER, PAGER>::FactorGraph(JobConfig _jobconfig):
  jobconfig(_jobconfig),
  variables(VariableScanner<DRIVER>(_jobconfig.frame_size_in_byte, _jobconfig.workdir + "/vars")),
  factors(FactorAccessor<DRIVER, PAGER>(_jobconfig.buffer_size_in_byte, _jobconfig.frame_size_in_byte, _jobconfig.workdir + "/facs"))
{  
  std::cout << "  | Loading variables..." << std::endl;
  std::cout << "    # var = " << this->variables.load(jobconfig.filename_variables) << std::endl;
  
  std::cout << "  | Loading factors..." << std::endl;
  std::cout << "    # fac = " << this->factors.load(jobconfig.filename_factors) << std::endl;
  
  std::cout << "  | Loading models..." << std::endl;
  std::cout << "    # mod = " << ModelAccessor::load(jobconfig.filename_models) << std::endl;
}

template<class DRIVER, class PAGER>
FactorGraph<DRIVER, PAGER>::~FactorGraph()
{

}

// The following lines are used as workaround to seperated implementation and declration of template class
template class FactorGraph<Driver_FILE, int>;


