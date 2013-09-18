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


#ifndef FACTORFACTORY_H
#define FACTORFACTORY_H

#include <common/include.h>
#include <fg/variable.h>

#include <adhoc/factor_lda.h>
#include <adhoc/factor_booleandisjunction.h>
#include <adhoc/factor_sum.h>
#include <adhoc/factor_avg.h>
#include <adhoc/factor_join.h>
#include <adhoc/factor_valueconjunction.h>

class FactorFactory
{
  
public:  
  
  static int get_factor_size(int ffid, long sizeaux){

    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::factor_size(sizeaux);
    if(ffid == 1) return Factor_BooleanDisjunction::factor_size(sizeaux);
    if(ffid == 2) return Factor_Sum::factor_size(sizeaux);
    if(ffid == 3) return Factor_Avg::factor_size(sizeaux);
    if(ffid == 4) return Factor_Join::factor_size(sizeaux);
    
    if(ffid == 10) return Factor_ValueConjunction::factor_size(sizeaux);
      
    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
  static void load_factor(int ffid, char * state, long sizeaux, int aux){
   
    assert(aux != 0 && "Parameter starts from 1~\\infty. Or -1~-\\infty where the negative sign encodes other information.");
    
    (*reinterpret_cast<int*>(&state[0])) = ffid;
    
    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::load_factor(state, sizeaux, aux);
    if(ffid == 1) return Factor_BooleanDisjunction::load_factor(state, sizeaux, aux);
    if(ffid == 2) return Factor_Sum::load_factor(state, sizeaux, aux);
    if(ffid == 3) return Factor_Avg::load_factor(state, sizeaux, aux);
    if(ffid == 4) return Factor_Join::load_factor(state, sizeaux, aux);
    
    if(ffid == 10) return Factor_ValueConjunction::load_factor(state, sizeaux, aux);
    
    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
  static void init_factor(char * state, long i_factor, Variable * var){
   
    int ffid = (*reinterpret_cast<int*>(&state[0]));
    
    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::init_factor(state, i_factor, var);
    if(ffid == 1) return Factor_BooleanDisjunction::init_factor(state, i_factor, var);
    if(ffid == 2) return Factor_Sum::init_factor(state, i_factor, var);
    if(ffid == 3) return Factor_Avg::init_factor(state, i_factor, var);
    if(ffid == 4) return Factor_Join::init_factor(state, i_factor, var);
    
    if(ffid == 10) return Factor_ValueConjunction::init_factor(state, i_factor, var);
    
    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
  static double potential_factor(char * state, long i_factor, Variable * propose, Variable * original){
    int ffid = (*reinterpret_cast<int*>(&state[0]));
    
    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::potential_factor(state, i_factor, propose, original);
    if(ffid == 1) return Factor_BooleanDisjunction::potential_factor(state, i_factor, propose, original);
    if(ffid == 2) return Factor_Sum::potential_factor(state, i_factor, propose, original);
    if(ffid == 3) return Factor_Avg::potential_factor(state, i_factor, propose, original);
    if(ffid == 4) return Factor_Join::potential_factor(state, i_factor, propose, original);
    
    if(ffid == 10) return Factor_ValueConjunction::potential_factor(state, i_factor, propose, original);
	
    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
  static void update_factor(char * state, long i_factor, Variable * propose, Variable * original){
    int ffid = (*reinterpret_cast<int*>(&state[0]));
    
    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::update_factor(state, i_factor, propose, original);
    if(ffid == 1) return Factor_BooleanDisjunction::update_factor(state, i_factor, propose, original);
    if(ffid == 2) return Factor_Sum::update_factor(state, i_factor, propose, original);
    if(ffid == 3) return Factor_Avg::update_factor(state, i_factor, propose, original);
    if(ffid == 4) return Factor_Join::update_factor(state, i_factor, propose, original);
    
    if(ffid == 10) return Factor_ValueConjunction::update_factor(state, i_factor, propose, original);

    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
  static void update_model(char * state, long i_factor, Variable * propose, Variable * original){
    int ffid = (*reinterpret_cast<int*>(&state[0]));
    
    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::update_model(state, i_factor, propose, original);
    if(ffid == 1) return Factor_BooleanDisjunction::update_model(state, i_factor, propose, original);
    if(ffid == 2) return Factor_Sum::update_model(state, i_factor, propose, original);
    if(ffid == 3) return Factor_Avg::update_model(state, i_factor, propose, original);
    if(ffid == 4) return Factor_Join::update_model(state, i_factor, propose, original);
    
    if(ffid == 10) return Factor_ValueConjunction::update_model(state, i_factor, propose, original);

    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
  static bool variable_generate(char * state, long i_factor, Variable * propose, Variable * original){
    int ffid = (*reinterpret_cast<int*>(&state[0]));
    
    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::variable_generate(state, i_factor, propose, original);
    if(ffid == 1) return Factor_BooleanDisjunction::variable_generate(state, i_factor, propose, original);
    if(ffid == 2) return Factor_Sum::variable_generate(state, i_factor, propose, original);
    if(ffid == 3) return Factor_Avg::variable_generate(state, i_factor, propose, original);
    if(ffid == 4) return Factor_Join::variable_generate(state, i_factor, propose, original);
    
    if(ffid == 10) return Factor_ValueConjunction::variable_generate(state, i_factor, propose, original);
	
    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
};

#endif // FACTORFACTORY_H







