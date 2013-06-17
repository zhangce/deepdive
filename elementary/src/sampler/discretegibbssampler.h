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


#ifndef DISCRETEGIBBSSAMPLER_H
#define DISCRETEGIBBSSAMPLER_H

#include <adhoc/factorfactory.h>

template<class DRIVER, class PAGER>
class DiscreteGibbsSampler
{
public:
  
  static int sample(FactorAccessor<DRIVER, PAGER> * factors, Variable * pvar,  Variable * const pvar_ori, double* p_double1000buffer, bool is_log_scale){
    
    long fid;
    
    for(int newvalue=pvar->lower;newvalue<=pvar->upper;newvalue++){
      if(!is_log_scale){
	p_double1000buffer[newvalue] = 1.0;
      }else{
	p_double1000buffer[newvalue] = 0.0;
      }
    }
    
    for(long i_factor=0;i_factor<pvar->nfactor;i_factor++){
      fid = *pvar->get_i_fid(i_factor);   
      char * fstate = factors->get_record(fid);
      
      // propose
      for(int newvalue=pvar->lower;newvalue<=pvar->upper;newvalue++){
	
	//*pvar->get_i_value(0) = newvalue;
	pvar->cvalue = newvalue;
	
	double potential = FactorFactory::potential_factor(fstate, i_factor, pvar, pvar_ori);
	
	if(!is_log_scale){
	  p_double1000buffer[newvalue] *= potential;
	}else{
	  p_double1000buffer[newvalue] += potential;
	}
      }
      
      factors->release_record(fid);
    }
    
    double sum = 0;
    if(!is_log_scale){
      sum = 0;
    }else{
      sum = -1000000;
    }
    
    for(int newvalue=pvar->lower;newvalue<=pvar->upper;newvalue++){
      if(!is_log_scale){
	sum += p_double1000buffer[newvalue];
      }else{
	sum = logadd(sum, p_double1000buffer[newvalue]);
      }
    }
    
    double r = drand48();
    double accum = 0;
    if(!is_log_scale){
      accum = 0;
    }else{
      accum = -1000000;
    }
    
    double ratio = 0;
    int topick = -1;
    for(int newvalue=pvar->lower;newvalue<=pvar->upper;newvalue++){
      if(!is_log_scale){
	ratio += p_double1000buffer[newvalue] / sum;
      }else{
	ratio += exp(p_double1000buffer[newvalue] - sum);
      }
      if(r <= ratio){
	topick = newvalue;
	break;
      }
    }
    assert(topick != -1);
    
    //*pvar->get_i_value(0) = topick;
    pvar->cvalue = topick;
    
    //if(topick != *pvar_ori->get_i_value(0)){
    if(topick != pvar_ori->cvalue){
      return 1;
    }else{
      return 0;
    }
    
  }
  
};

#endif // DISCRETEGIBBSSAMPLER_H
