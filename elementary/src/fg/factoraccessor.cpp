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


#include "factoraccessor.h"


template<class DRIVER, class PAGER>
long FactorAccessor<DRIVER, PAGER>::load(std::string _filename_factors)
{
  std::ifstream fin(_filename_factors.c_str());
  long fid, sizeaux;
  int ffid, modelid;
  
  long nfactor = 0;
  
  int page_id, offset_id;
  
  while(fin >> fid >> sizeaux >> ffid >> modelid){
    this->add_record(fid, FactorFactory::get_factor_size(ffid, sizeaux), page_id, offset_id);    
    FactorFactory::load_factor(ffid, this->get_record(fid), sizeaux, modelid);
    nfactor = nfactor + 1;
  }
  
  fin.close();
  return nfactor;
}



template<class DRIVER, class PAGER>
FactorAccessor<DRIVER, PAGER>::FactorAccessor(const long int& _buffer_size_in_byte, const int& _frame_size_in_byte, std::string _conn_string) : 
  VarLenKV<DRIVER, PAGER>(_buffer_size_in_byte, _frame_size_in_byte, _conn_string)
{

}

template<class DRIVER, class PAGER>
FactorAccessor<DRIVER, PAGER>::~FactorAccessor()
{

}

// The following lines are used as workaround to seperated implementation and declration of template class
template class FactorAccessor<Driver_FILE, int>;

