#include "timer.h"

#ifdef __MACH__
int clock_gettime(int /*clk_id*/, struct timespec* t){
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}

#endif
