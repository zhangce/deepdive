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


#ifndef UNIFORMMETROPOLISHASTINGS_H
#define UNIFORMMETROPOLISHASTINGS_H
#include <fg/variable.h>
#include <adhoc/factorfactory.h>

template<class DRIVER, class PAGER>
class UniformMetropolisHastings
{
public:
  
  static int get_potential(FactorAccessor<DRIVER, PAGER> * factors, Variable * const pvar, bool is_log_scale){
    long fid;
    double potential;
    if(!is_log_scale){
      potential = 1.0;
    }else{
      potential = 0.0;
    }
    
    for(long i_factor=0;i_factor<pvar->nfactor;i_factor++){
      fid = *pvar->get_i_fid(i_factor);   
      char * fstate = factors->get_record(fid);
      
      if(!is_log_scale){
	potential *= FactorFactory::potential_factor(fstate, i_factor, pvar, pvar);;
      }else{
	potential += FactorFactory::potential_factor(fstate, i_factor, pvar, pvar);;
      }
      
      factors->release_record(fid);
    }
    
  }
  
  static int sample(FactorAccessor<DRIVER, PAGER> * factors, Variable * pvar,  Variable * const pvar_ori, double* p_double1000buffer, bool is_log_scale){
    
    long fid;
    
    double current_potential;
    double proposed_potential;
    int ori_cvalue;
    double ori_rvalue;
    bool accept;
    double r;
    
    for(int ntrail=0;ntrail<100;ntrail++){
      current_potential = get_potential(factors, pvar, is_log_scale);
      ori_cvalue = pvar->cvalue;
      ori_rvalue = pvar->rvalue;
      
      // propose
      if(pvar->dtype == 'C'){
	pvar->cvalue = int(drand48()*(pvar->upper - pvar->lower + 1) + pvar->lower);
      }else if(pvar->dtype == 'R'){
	pvar->rvalue = drand48()*(pvar->upper - pvar->lower) + pvar->lower;
	
      }else if(pvar->dtype == 'M'){
	assert(false);
      }else{
	assert(false);
      }
      
      proposed_potential = get_potential(factors, pvar, is_log_scale);
      r = drand48();
      
      if(is_log_scale){
	accept = (r < exp(proposed_potential - current_potential));
      }else{
	accept = (r < proposed_potential/current_potential);
      }
      
      if(!accept){
	pvar->cvalue = ori_cvalue;
	pvar->rvalue = ori_rvalue;
      }
    }
    
    if(pvar->cvalue != pvar_ori->cvalue || pvar->rvalue != pvar_ori->rvalue){
      return 1;
    }else{
      return 0;
    }

  }
};

#endif // UNIFORMMETROPOLISHASTINGS_H
