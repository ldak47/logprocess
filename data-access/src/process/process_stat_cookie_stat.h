#ifndef DATAACCESS_PROCESS_COOKIE_STAT_H
#define DATAACCESS_PROCESS_COOKIE_STAT_H
#include "redis.h"
#include "libconfig.h++"
#include "time_trans.h"
#include <map>
#include <mutex>
#include <memory>
#include <string>
#include "glog/logging.h"
#include "gflags/gflags.h"

namespace dataprocess {

using query_record = std::tuple<time_t, std::string>;
    
class CookieStat {
    static std::mutex ckmtx;
    static std::unique_ptr<common::RedisCli> ckrd_handle;

public:
    CookieStat(){}
    ~CookieStat(){}
    static void UpdateCookieStat(const std::map<std::string, std::string> &kv);
    static void Search(const std::string &cookie, std::vector<query_record> &records, time_t st, time_t ed);
};

};
#endif
