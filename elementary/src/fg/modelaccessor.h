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


#ifndef MODELACCESSOR_H
#define MODELACCESSOR_H

#include <common/include.h>

class ModelAccessor
{
public:
  static std::vector<double> model;

  static std::vector<bool> model_isfixed;
  
  static double load(std::string _filename_model);
  
  static double sgd_step_size;
  
  static bool start_tally;
  
  static bool is_log_system;
  
  ModelAccessor(){}
  
};

#endif // MODELACCESSOR_H
