#ifndef DATAACCESS_COMMON_REDIS_H
#define DATAACCESS_COMMON_REDIS_H
#include <hiredis/hiredis.h>
#include "libconfig.h++"
#include "gflags/gflags.h"
#include <mutex>
#include <map>
#include <vector>
#include <strings.h>
#include <string>

namespace common {
    
class RedisCli {
    std::string ip_;
    int port_;
    int default_ttl_;
    redisContext *ctx_;

    std::mutex mtx;

    int err_;
    std::string errmsg_;

public:
    RedisCli(const libconfig::Setting &redis_cfg);
    RedisCli(std::string ipaddr, int port);
    ~RedisCli();

    int GetError (std::string &err) {
        err = errmsg_;
        return err_;
    }

    bool Set(const std::string &key, const std::string &value, int ttl = 500);
    bool Get(const std::string &key, std::string &value);
    bool Incr(const std::string &key);
    bool Exists(const std::string &key);
    bool Zset(const std::string &key, const std::string &value, int64_t score);
    bool Zcount(const std::string &key, uint64_t &count, int64_t start_score, int64_t end_score);
    bool Zrange(const std::string &key, std::vector<std::string> &values, int64_t start = 0, int64_t end = -1, bool direct = true);
    bool ZrangeByScore(const std::string &key, std::vector<std::string> &values, std::string start_score = "", std::string end_score = "", bool direct = true);
    bool ZrangeByScoreWithScore(const std::string &key, std::map<std::string, std::string> &values, std::string start_score = "", std::string end_score = "", bool direct = true);
    bool ZRem(const std::string &key, int64_t st_score = 0, int64_t ed_score = -1, bool type = true); //false: ZREMRANGEBYSCORE true: ZREMRANGEBYRANK
    
    
};

    

};
#endif
