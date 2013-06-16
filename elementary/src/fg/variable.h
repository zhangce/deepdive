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


#ifndef VARIABLE_H
#define VARIABLE_H

#include <common/include.h>

class Variable{
  
public:
  
  long vid;
  
  char dtype;
  char vtype;
  char stype;
  
  double lower;
  double upper;
  long nfactor;
  
  int cvalue;
  double rvalue;
  
  char content[1];
  
  long * get_i_fid(const long & i);
  
  int * get_i_f_group(const long &  i);
  
  int * get_i_f_pos(const long &  i);
  
  int * get_i_f_aux(const long &  i);

  double * get_i_value(const long &  i);  
  
  static int size(const char & type, const double & lower, const double & upper, const long & nfactor){
    if(type == 'M'){
      assert(false);
      //return sizeof(Variable) + nfactor * (sizeof(long) + 3*sizeof(int)) + sizeof(double)*lower*upper;
    }else{
      return sizeof(Variable) + nfactor * (sizeof(long) + 3*sizeof(int)) + sizeof(double);
    }
  }
  
};

#endif // VARIABLE_H
