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


#include "factorgraph.h"

template<class DRIVER, class PAGER>
double process_variable_init(char * buf, void * ppara){
  
  FactorAccessor<DRIVER, PAGER> * factors = reinterpret_cast<FactorAccessor<DRIVER, PAGER>*>(ppara);
  Variable * pvar = reinterpret_cast<Variable*>(buf);
  
  long fid;
  
  for(long i_factor=0;i_factor<pvar->nfactor;i_factor++){
    fid = *pvar->get_i_fid(i_factor);
    FactorFactory::init_factor(factors->get_record(fid), i_factor, pvar);
  }
  
}

template<class DRIVER, class PAGER>
void FactorGraph<DRIVER, PAGER>::init()
{
  Timer t;
  this->variables.scan(&process_variable_init<DRIVER, PAGER>, &this->factors);
  std::cout << t.elapsed() << std::endl;
}


template<class DRIVER, class PAGER>
void FactorGraph<DRIVER, PAGER>::sample()
{

}

template<class DRIVER, class PAGER>
FactorGraph<DRIVER, PAGER>::FactorGraph(std::string _filename_variables, std::string _filename_factors, const long & _buffer_size_in_byte, const int & _framesize_in_byte):
  filename_variables(_filename_variables),
  filename_factors(_filename_factors),
  variables(VariableScanner<DRIVER>(_framesize_in_byte, "/tmp/vars")),
  factors(FactorAccessor<DRIVER, PAGER>(_buffer_size_in_byte, _framesize_in_byte, "/tmp/facs"))
{
  std::cout << "  | Loading variables..." << std::endl;
  std::cout << "    # var = " << this->variables.load(filename_variables) << std::endl;
  
  std::cout << "  | Loading factors..." << std::endl;
  std::cout << "    # fac = " << this->factors.load(filename_factors) << std::endl;
}

template<class DRIVER, class PAGER>
FactorGraph<DRIVER, PAGER>::~FactorGraph()
{

}

// The following lines are used as workaround to seperated implementation and declration of template class
template class FactorGraph<Driver_FILE, int>;


