#ifndef DATAACCESS_COMMON_TIME_H
#define DATAACCESS_COMMON_TIME_H
#include <ctime>
#include <string>
#include <cctype>
#include <memory.h>
#include <algorithm>

time_t strTime2Unixtime (std::string strTime);
std::string unixTime2Str(time_t unixtime);












#endif
