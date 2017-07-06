#include "util.h"

namespace common {
    
int64_t getime_micros () {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return static_cast<int64_t>(ts.tv_sec) * 1000 * 1000 + static_cast<int64_t>(ts.tv_nsec)/100;
}

};
