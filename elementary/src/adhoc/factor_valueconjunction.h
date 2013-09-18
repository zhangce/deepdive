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


#ifndef FACTOR_VALUECONJUNCTION_H
#define FACTOR_VALUECONJUNCTION_H


#include <fg/modelaccessor.h>
#include <fg/variable.h>

class ValueConjunction_Factor_State{
public:
  int ffid;
  int aux;
  int nsat;
  int nvar;
};

class Factor_ValueConjunction
{
  
public:
  
  static int factor_size(long sizeaux){
    return sizeof(int) + sizeof(int) + sizeof(int) + sizeof(int);
  }
  
  static void load_factor(char * state, long sizeaux, int aux){
    ValueConjunction_Factor_State * pstate = reinterpret_cast<ValueConjunction_Factor_State*>(state);
    pstate->aux = aux;
    pstate->nsat = 0;
    pstate->nvar = sizeaux;
  }
  
  static void init_factor(char * state, long i_factor, Variable * var){
    ValueConjunction_Factor_State * pstate = reinterpret_cast<ValueConjunction_Factor_State*>(state);
    if(var->cvalue == *var->get_i_f_aux(i_factor)){
      pstate->nsat ++;
    }
    assert(var->dtype !='R');
  }
  
  static double potential_factor(char * state, long i_factor, Variable * propose, Variable * original){
    ValueConjunction_Factor_State * pstate = reinterpret_cast<ValueConjunction_Factor_State*>(state);
    
    int hypothesis_nsat = pstate->nsat;
    if(*propose->get_i_f_aux(i_factor) == propose->cvalue){
	hypothesis_nsat ++;
    }
    if(*original->get_i_f_aux(i_factor) == original->cvalue){
	hypothesis_nsat --;
    }
    
    assert(hypothesis_nsat >= 0);
    if(hypothesis_nsat != pstate->nvar){
      return 0.0;
    }else{
      return ModelAccessor::model[pstate->aux];
    }
  }
  
  static void update_factor(char * state, long i_factor, Variable * propose, Variable * original){
     ValueConjunction_Factor_State * pstate = reinterpret_cast<ValueConjunction_Factor_State*>(state);
     if(*propose->get_i_f_aux(i_factor) == propose->cvalue){
	pstate->nsat ++;
     }
     if(*original->get_i_f_aux(i_factor) == original->cvalue){
	pstate->nsat --;
     }
     assert(pstate->nsat >= 0);
  }
  
  static bool variable_generate(char * state, long i_factor, Variable * propose, Variable * original){
    return false;
  }
  
  static void update_model(char * state, long i_factor, Variable * propose, Variable * original){
    
    ValueConjunction_Factor_State * pstate = reinterpret_cast<ValueConjunction_Factor_State*>(state);
    
    if(ModelAccessor::model_isfixed[pstate->aux] == true){
      return;
    }
    
    int hypothesis_nsat = pstate->nsat;
    if(*propose->get_i_f_aux(i_factor) == propose->cvalue){
	hypothesis_nsat ++;
    }
    if(*original->get_i_f_aux(i_factor) == original->cvalue){
	hypothesis_nsat --;
    }
    
    if(hypothesis_nsat != pstate->nvar && pstate->nsat == pstate->nvar){
      ModelAccessor::model[pstate->aux] += ModelAccessor::sgd_step_size;
    }else if(hypothesis_nsat == pstate->nvar && pstate->nsat != pstate->nvar){
      ModelAccessor::model[pstate->aux] -= ModelAccessor::sgd_step_size;    
    }
    
    return;
  }
  
};



#endif // FACTOR_VALUECONJUNCTION_H
