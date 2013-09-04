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


#ifndef FACTOR_AVG_H
#define FACTOR_AVG_H

#include <fg/modelaccessor.h>
#include <fg/variable.h>

class Avg_Factor_State{
public:
  int ffid;
  int aux;
  double running_sum;
  double running_auxsum;
  long outputid;
};

class Factor_Avg
{
public:
  
  static int factor_size(long sizeaux){
    return sizeof(int) + sizeof(int) + 2*sizeof(double) + sizeof(long);
  }
  
  static void load_factor(char * state, long sizeaux, int aux){
    Avg_Factor_State * pstate = reinterpret_cast<Avg_Factor_State*>(state);
    pstate->aux = aux;
    pstate->running_sum = 0;
    pstate->running_auxsum = 0;
    pstate->outputid = -1;
  }
  
  static void init_factor(char * state, long i_factor, Variable * var){
    Avg_Factor_State * pstate = reinterpret_cast<Avg_Factor_State*>(state);
    
    if((*var->get_i_f_pos(i_factor)) == 0){
      assert(var->dtype == 'R');
      assert(pstate->outputid == -1);
      pstate->outputid = var->vid;
      return;
    }
    
    if(var->dtype == 'C'){
      pstate->running_sum += var->cvalue;
      pstate->running_auxsum += var->cvalue * (*var->get_i_f_aux(i_factor));
    }else if(var->dtype == 'R'){
      pstate->running_auxsum += var->rvalue;
      pstate->running_auxsum += var->rvalue * (*var->get_i_f_aux(i_factor));
    }else if(var->dtype == 'M'){
      assert(false);
    }else{
      assert(false);
    }
  }
  
  static double potential_factor(char * state, long i_factor, Variable * propose, Variable * original){
     Avg_Factor_State * pstate = reinterpret_cast<Avg_Factor_State*>(state);
     return 0;
  }
  
  static void update_factor(char * state, long i_factor, Variable * propose, Variable * original){
    
     Avg_Factor_State * pstate = reinterpret_cast<Avg_Factor_State*>(state);
     
     if(original->vid == pstate->outputid){
       return; 
     }
     
     if(original->dtype == 'C'){	
	pstate->running_sum += propose->cvalue - original->cvalue;
	pstate->running_auxsum += (propose->cvalue - original->cvalue) * (*original->get_i_f_aux(i_factor));
     }else if(original->dtype == 'R'){
	pstate->running_sum += propose->rvalue - original->rvalue;
	pstate->running_auxsum += (propose->rvalue - original->rvalue) * (*original->get_i_f_aux(i_factor));
     }else if(original->dtype == 'M'){
	assert(false);
     }else{
	assert(false);
    }
  }
  
  static bool variable_generate(char * state, long i_factor, Variable * propose, Variable * original){
    
    Avg_Factor_State * pstate = reinterpret_cast<Avg_Factor_State*>(state);
    
    assert(original->vid == pstate->outputid);
    
    if(pstate->running_sum == 0){
      propose->rvalue = 0;
    }else{
      propose->rvalue = pstate->running_auxsum/pstate->running_sum;
    }
    
    return true;
  }
  
  static void update_model(char * state, long i_factor, Variable * propose, Variable * original){
    return;
  }
  
};

#endif // FACTOR_AVG_H







