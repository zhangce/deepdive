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

class FactorFactory
{
  
public:
  static int get_factor_size(int ffid, long sizeaux){

    // LDA FACTOR
    if(ffid == 0) return sizeof(int) + Factor_LDA::factor_size(sizeaux);
      
    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
  static void load_factor(int ffid, char * state, long sizeaux, int aux){
   
    (*reinterpret_cast<int*>(&state[0])) = ffid;
    
    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::load_factor(&state[sizeof(int)], sizeaux, aux);
	
    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  
  static void init_factor(char * state, long i_factor, Variable * var){
   
    int ffid = (*reinterpret_cast<int*>(&state[0]));
    
    // LDA FACTOR
    if(ffid == 0) return Factor_LDA::init_factor(&state[sizeof(int)], i_factor, var);
	
    assert(false && "FactorFactory: FactorID not in the catalog");
  }
  

};

#endif // FACTORFACTORY_H







