#include "time_trans.h"

using namespace std;

inline std::string ltrim (std::string &str) {
    string::iterator p = find_if(str.begin(), str.end(), not1(ptr_fun<int, int>(isspace)));
    str.erase(str.begin(), p);
    return str;
}

inline std::string rtrim (std::string &str) {
    string::reverse_iterator p = find_if(str.rbegin(), str.rend(), not1(ptr_fun<int , int>(isspace)));
    str.erase(p.base(), str.end());
    return str;
}

inline std::string trim (std::string &str) {
    ltrim(str);
    rtrim(str);
    return str;
}

time_t strTime2Unixtime (std::string strTime) {
    std::string str = strTime;
    trim(str);
    struct tm tm;
    memset(&tm, 0, sizeof(tm));
    
    sscanf(str.c_str(), "%d-%d-%d %d:%d:%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    tm.tm_year -= 1900;
    tm.tm_mon--;
    return mktime(&tm);
}

std::string unixTime2Str(time_t unixtime) {
    time_t utime = unixtime;
    char buf[100] = {0};
    strftime(buf, sizeof(buf) - 1, "%Y-%m-%d %H:%M:%S", localtime((time_t *)&utime));
    std::string strtime = buf;
    return strtime;
}



