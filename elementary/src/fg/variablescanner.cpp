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


#include "variablescanner.h"

  /**
   * Format of variables file:
   * 
   *    VID(long) #DTYPE(char, C, R, M) #VTYPE(char E, Q) #STYPE(char S, G) LOWERBOUND(double) UPPERBOUND(double) NFACTOR(long) [FID(long), GROUP(int), POS(int), AUX(int)...] [INITVALUES(double)...]
   * 
   */
template<class DRIVER>
long VariableScanner<DRIVER>::load(std::string _filename_variables)
{
  std::ifstream fin(_filename_variables.c_str());
  long vid, nfactor, fid;
  char dtype, vtype, stype;
  double lower, upper;
  int group, pos, aux;
  double value;
  long nvariable = 0;
  int variable_size_in_byte;
  
  long nassignment;
  
  while(fin >> vid >> dtype >> vtype >> stype >> lower >> upper >> nfactor){
    
    variable_size_in_byte = Variable::size(dtype, lower, upper, nfactor);
    
    Variable * pvar = reinterpret_cast<Variable*>(this->push_record(variable_size_in_byte));
    
    pvar->vid = vid;
    pvar->dtype = dtype;
    pvar->vtype = vtype;
    pvar->stype = stype;
    pvar->lower = lower;
    pvar->upper = upper;
    pvar->nfactor = nfactor;

    for(long i=0;i<nfactor;i++){
      fin >> (*(pvar->get_i_fid(i))) >> (*(pvar->get_i_f_group(i))) >> (*(pvar->get_i_f_pos(i))) >> (*(pvar->get_i_f_aux(i)));
    }
   
    if(dtype == 'C' || dtype == 'R'){
      fin >> (*(pvar->get_i_value(0)));
    }else if(dtype == 'M'){
      assert(false);
    }else{
      assert(false); 
    }
    
    nvariable = nvariable + 1;
  }
  
  fin.close();
  return nvariable;
}

template<class DRIVER>
VariableScanner<DRIVER>::VariableScanner(int _framesize_in_byte, std::string _conn_string): Scanner<DRIVER>(_framesize_in_byte, _conn_string){
  
}

template<class DRIVER>
VariableScanner<DRIVER>::~VariableScanner()
{

}

// The following lines are used as workaround to seperated implementation and declration of template class
template class VariableScanner<Driver_FILE>;


