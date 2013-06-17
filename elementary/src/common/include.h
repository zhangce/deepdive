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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef INCLUDE_H
#define INCLUDE_H

#include <stdlib.h>

#include <fstream>

#include <iostream>
#include <vector>
#include <map>
#include <assert.h>
#include <string>

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <malloc.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include <math.h>

#include "common/types.h"
#include "common/timer.h"

#include <sstream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
            ( std::ostringstream() << std::dec << x ) ).str()
            
            
#define LOG_2   0.693147180559945
#define MINUS_LOG_THRESHOLD   -18.42

inline bool fast_exact_is_equal(double a, double b){
  return (a <= b && b <= a);
}

inline double logadd(double log_a, double log_b){
  
  if (log_a < log_b)
  { // swap them
      double tmp = log_a;
      log_a = log_b;
      log_b = tmp;
  } else if (fast_exact_is_equal(log_a, log_b)) {
      // Special case when log_a == log_b. In particular this works when both
      // log_a and log_b are (+-) INFINITY: it will return (+-) INFINITY
      // instead of NaN.
      return LOG_2 + log_a;
  }
  double negative_absolute_difference = log_b - log_a;
  if (negative_absolute_difference < MINUS_LOG_THRESHOLD)
    return (log_a);
  return (log_a + log1p(exp(negative_absolute_difference)));
  
}



#endif // INCLUDE_H
