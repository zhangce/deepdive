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


#include "modelaccessor.h"

std::vector<double> ModelAccessor::model;
std::vector<bool> ModelAccessor::model_isfixed;

double ModelAccessor::sgd_step_size;

bool ModelAccessor::start_tally;

double ModelAccessor::load(std::string _filename_model)
{
  std::ifstream fin(_filename_model.c_str());
  int pid;
  double number;
  bool fixed;
  int nline = 0;
  
  while(fin >> pid >> number >> fixed){
    while(pid >= ModelAccessor::model.size()){
      ModelAccessor::model.push_back(0.0);
      ModelAccessor::model_isfixed.push_back(true);
    }
    ModelAccessor::model[pid] = number;
    ModelAccessor::model_isfixed[pid] = fixed;
    nline++;
  }
  
  fin.close();
  return nline;
  
}
  