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


#ifndef FACTORACCESSOR_H
#define FACTORACCESSOR_H

#include <common/include.h>
#include <io/varlenkv.h>
#include <adhoc/factorfactory.h>

template<class DRIVER, class PAGER>
class FactorAccessor : public VarLenKV<DRIVER, PAGER>
{

public:

  /**
   * Format of factors file:
   * 
   *    FID(long) #SIZEAUX(long) FACTORFUNCTION(int) MODELID(int)
   * 
   */
  long load(std::string _filename_factors);

  FactorAccessor(const long int& _buffer_size_in_byte, const int& _frame_size_in_byte, std::string _conn_string);
  virtual ~FactorAccessor();
};

#endif // FACTORACCESSOR_H

