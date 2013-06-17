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


#ifndef FACTOR_BOOLEANDISJUNCTION_H
#define FACTOR_BOOLEANDISJUNCTION_H


#include <fg/modelaccessor.h>
#include <fg/variable.h>

class BooleanDisjunction_Factor_State{
public:
  int ffid;
  int aux;
  int nsat;
};

class Factor_BooleanDisjunction
{
  
public:
  
  static int factor_size(long sizeaux){
    return sizeof(int) + sizeof(int) + sizeof(int);
  }
  
  static void load_factor(char * state, long sizeaux, int aux){
    BooleanDisjunction_Factor_State * pstate = reinterpret_cast<BooleanDisjunction_Factor_State*>(state);
    pstate->aux = aux;
    pstate->nsat = 0;
  }
  
  static void init_factor(char * state, long i_factor, Variable * var){
    BooleanDisjunction_Factor_State * pstate = reinterpret_cast<BooleanDisjunction_Factor_State*>(state);
    if(var->cvalue == 0 && var->get_i_f_aux(i_factor) < 0){
      pstate->nsat ++;
    }
    if(var->cvalue == 1 && var->get_i_f_aux(i_factor) > 0){
      pstate->nsat ++;
    }
  }
  
  static double potential_factor(char * state, long i_factor, Variable * propose, Variable * original){
    BooleanDisjunction_Factor_State * pstate = reinterpret_cast<BooleanDisjunction_Factor_State*>(state);
    
    int hypothesis_nsat = pstate->nsat;
    if(propose->get_i_f_aux(i_factor) > 0){
      if(propose->cvalue == 0 && original->cvalue == 1){
	hypothesis_nsat --;
      }
      if(propose->cvalue == 1 && original->cvalue == 0){
	hypothesis_nsat ++;
      }
    }else{
      if(propose->cvalue == 0 && original->cvalue == 1){
	hypothesis_nsat ++;
      }
      if(propose->cvalue == 1 && original->cvalue == 0){
	hypothesis_nsat --;
      }
    }
    
    assert(hypothesis_nsat >= 0);
    if(hypothesis_nsat == 0){
      return 0.0;
    }else{
      return ModelAccessor::model[pstate->aux];
    }
    
  }
  
  static void update_factor(char * state, long i_factor, Variable * propose, Variable * original){
     BooleanDisjunction_Factor_State * pstate = reinterpret_cast<BooleanDisjunction_Factor_State*>(state);
     if(propose->get_i_f_aux(i_factor) > 0){
      if(propose->cvalue == 0 && original->cvalue == 1){
	pstate->nsat --;
      }
      if(propose->cvalue == 1 && original->cvalue == 0){
	pstate->nsat ++;
      }
    }else{
      if(propose->cvalue == 0 && original->cvalue == 1){
	pstate->nsat ++;
      }
      if(propose->cvalue == 1 && original->cvalue == 0){
	pstate->nsat --;
      }
    }
    assert(pstate->nsat >= 0);
  }
  
  static bool variable_generate(char * state, long i_factor, Variable * propose, Variable * original){
    return false;
  }
  
  static void update_model(char * state, long i_factor, Variable * propose, Variable * original){
    
    BooleanDisjunction_Factor_State * pstate = reinterpret_cast<BooleanDisjunction_Factor_State*>(state);
    
    if(ModelAccessor::model_isfixed[pstate->aux] == true){
      return;
    }
    
    int hypothesis_nsat = pstate->nsat;
    if(propose->get_i_f_aux(i_factor) > 0){
      if(propose->cvalue == 0 && original->cvalue == 1){
	hypothesis_nsat --;
      }
      if(propose->cvalue == 1 && original->cvalue == 0){
	hypothesis_nsat ++;
      }
    }else{
      if(propose->cvalue == 0 && original->cvalue == 1){
	hypothesis_nsat ++;
      }
      if(propose->cvalue == 1 && original->cvalue == 0){
	hypothesis_nsat --;
      }
    }
    
    
    if(hypothesis_nsat == 0 && pstate->nsat > 0){
      ModelAccessor::model[pstate->aux] += ModelAccessor::sgd_step_size;
    }else if(hypothesis_nsat > 0 && pstate->nsat == 0){
      ModelAccessor::model[pstate->aux] -= ModelAccessor::sgd_step_size;    
    }
    
    return;
  }
  
};



#endif // FACTOR_BOOLEANDISJUNCTION_H
