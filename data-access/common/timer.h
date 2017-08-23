#ifndef DATAACCESS_COMMON_TIMER_H
#define DATAACCESS_COMMON_TIMER_H
#include <sys/time.h>
#include <iostream>

namespace common {

class TimeCalcer {
    struct timeval tm;
    void start() {
        gettimeofday(&tm, nullptr);
    }
    
public:
    TimeCalcer() {
        start();
    }
    ~TimeCalcer(){}
    void restart() {
        start();
    }

    double Calc() {
        struct timeval ctm;
        gettimeofday(&ctm, nullptr);
        uint32_t elaps = (ctm.tv_sec - tm.tv_sec) * 1000000 + (ctm.tv_usec - tm.tv_usec);
        double elaps_d = double(elaps)/double(1000000);
        start();
        return elaps_d;
    }
};

};
#endif
