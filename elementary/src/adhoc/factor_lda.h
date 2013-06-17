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


#ifndef FACTOR_LDA_H
#define FACTOR_LDA_H

#include <fg/modelaccessor.h>
#include <fg/variable.h>

class LDA_Factor_State{
public:
  int ffid;
  int aux;
  long counts[];
};

class Factor_LDA
{
public:
  
  static int factor_size(long sizeaux){
    return sizeof(int) + sizeof(int) + sizeof(long)*sizeaux;
  }
  
  static void load_factor(char * state, long sizeaux, int aux){
    LDA_Factor_State * pstate = reinterpret_cast<LDA_Factor_State*>(state);
    pstate->aux = aux;
    for(int i=0;i<sizeaux;i++){
      pstate->counts[i] = 0;
    }
  }
  
  static void init_factor(char * state, long i_factor, Variable * var){
    LDA_Factor_State * pstate = reinterpret_cast<LDA_Factor_State*>(state);
    //assert(int(*var->get_i_value(0)) >= 0);
    //pstate->counts[int(*var->get_i_value(0))] ++;
    pstate->counts[var->cvalue] ++;
  }
  
  static double potential_factor(char * state, long i_factor, Variable * propose, Variable * original){
     LDA_Factor_State * pstate = reinterpret_cast<LDA_Factor_State*>(state);
     if(pstate->aux > 0){
       return ModelAccessor::model[pstate->aux] * pstate->counts[propose->cvalue];
       //return ModelAccessor::model[pstate->aux] * pstate->counts[int(*propose->get_i_value(0))];
     }else{
       return 1.0/ModelAccessor::model[-pstate->aux]/pstate->counts[propose->cvalue];
       //return 1.0/ModelAccessor::model[-pstate->aux]/pstate->counts[int(*propose->get_i_value(0))];
    }
  }
  
  static void update_factor(char * state, long i_factor, Variable * propose, Variable * original){
     LDA_Factor_State * pstate = reinterpret_cast<LDA_Factor_State*>(state);
     
     //pstate->counts[int(*propose->get_i_value(0))] ++;    
     //pstate->counts[int(*original->get_i_value(0))] --;
  
     pstate->counts[propose->cvalue] ++;    
     pstate->counts[original->cvalue] --;
  }
  
  static bool variable_generate(char * state, long i_factor, Variable * propose, Variable * original){
    return false;
  }
  
  static void update_model(char * state, long i_factor, Variable * propose, Variable * original){
    return;
  }
  
};

#endif // FACTOR_LDA_H
