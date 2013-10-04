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


#ifndef TIMER_H
#define TIMER_H


#include <time.h>
#include <sys/time.h>


#ifdef __MACH__
int clock_gettime(int /*clk_id*/, struct timespec* t);
#define CLOCK_MONOTONIC 0
#endif


            
class Timer {
public:
                
                struct timespec _start;
                struct timespec _end;
                
                Timer(){
                    clock_gettime(CLOCK_MONOTONIC, &_start);
                }
                
                virtual ~Timer(){}
                
                inline void restart(){
                    clock_gettime(CLOCK_MONOTONIC, &_start);
                }
                
                inline float elapsed(){
                    clock_gettime(CLOCK_MONOTONIC, &_end);
                    return (_end.tv_sec - _start.tv_sec) + (_end.tv_nsec - _start.tv_nsec) / 1000000000.0;
                }
                
};
            
#endif // TIMER_H
