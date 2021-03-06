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


#ifndef FACTORGRAPH_H
#define FACTORGRAPH_H

#include <common/include.h>
#include <fg/variablescanner.h>
#include <fg/factoraccessor.h>
#include <fg/modelaccessor.h>

#include <common/jobconfig.h>

template<class DRIVER, class PAGER>
class FactorGraph
{

private:
  
  VariableScanner<DRIVER> variables;
  FactorAccessor<DRIVER, PAGER> factors;
  
  JobConfig jobconfig;
  
public:
  
  void init();
  
  void sample();
  
  FactorGraph(JobConfig _jobconfig);
  
  virtual ~FactorGraph();
};

#endif // FACTORGRAPH_H
