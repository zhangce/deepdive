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


#include "variable.h"

long* Variable::get_i_fid(const long &  i)
{
  return reinterpret_cast<long*>(&this->content[i*(sizeof(long)+3*sizeof(int))]);
}

int* Variable::get_i_f_group(const long &  i)
{
  return reinterpret_cast<int*>(&this->content[i*(sizeof(long)+3*sizeof(int)) + sizeof(long)]);
}

int* Variable::get_i_f_pos(const long &  i)
{
  return reinterpret_cast<int*>(&this->content[i*(sizeof(long)+3*sizeof(int)) + sizeof(long) + sizeof(int)]);
}

int* Variable::get_i_f_aux(const long &  i)
{
  return reinterpret_cast<int*>(&this->content[i*(sizeof(long)+3*sizeof(int)) + sizeof(long) + 2*sizeof(int)]);
}

double* Variable::get_i_value(const long &  i)
{
  //assert(false);
  return reinterpret_cast<double*>(&this->content[nfactor*(sizeof(long)+3*sizeof(int)) + i*sizeof(double)]);
}




