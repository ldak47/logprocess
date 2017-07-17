#ifndef DATAACCESS_COMMON_REDIS_H
#define DATAACCESS_COMMON_REDIS_H
#include <hiredis/hiredis.h>
#include "libconfig.h++"
#include "gflags/gflags.h"
#include <mutex>
#include <strings.h>

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
    ~RedisCli();

    int GetError (std::string &err) {
        err = errmsg_;
        return err_;
    }

    bool Set(const std::string &key, const std::string &value, 
             int &err, std::string &errmsg, int ttl = 50);
    bool Get(const std::string &key, std::string &value, 
             int &err, std::string &errmsg);
    bool Exists(const std::string &key, 
                int &err, std::string &errmsg);
};

    

};
#endif
