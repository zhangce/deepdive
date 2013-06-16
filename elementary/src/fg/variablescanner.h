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


#ifndef VARIABLESCANNER_H
#define VARIABLESCANNER_H

#include <io/scanner.h>
#include <fg/variable.h>

template<class DRIVER>
class VariableScanner : public Scanner<DRIVER>
{

public:
  
  /**
   * Format of variables file:
   * 
   *    VID(long) #TYPE(char, C, R, M) LOWERBOUND(double) UPPERBOUND(double) NFACTOR(long) [FID(long), GROUP(int), POS(int), AUX(int)...] [INITVALUES(double)...]
   * 
   */
  long load(std::string _filename_variables);
    
  VariableScanner(int _framesize_in_byte, std::string _conn_string);
  
  virtual ~VariableScanner();
};

#endif // VARIABLESCANNER_H
