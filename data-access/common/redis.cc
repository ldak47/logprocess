#include "redis.h"

namespace common {

RedisCli::RedisCli (const libconfig::Setting &redis_cfg):ctx_(nullptr), err_(0), errmsg_("") {
    try {
        redis_cfg.lookupValue("ttl", default_ttl_);
        redis_cfg.lookupValue("ipaddr", ip_);
        redis_cfg.lookupValue("port", port_);
    } catch (std::exception &err) {
        return;
    }

    ctx_ = redisConnect(ip_.c_str(), port_);
    if (ctx_->err) {
        errmsg_ = ctx_->errstr;
        err_ = ctx_->err;
        redisFree(ctx_);
    }
}

RedisCli::~RedisCli () {
    if (ctx_) {
        redisFree(ctx_);
    }
}

bool RedisCli::Set (const std::string &key, const std::string &value, 
                    int &err, std::string &errmsg, int ttl) {
    if (default_ttl_ > 0) {
        ttl = default_ttl_;
    }
    err = 0;
    errmsg = "";
    std::string setcmd = "set " + key + " " + value;
    std::string ttlcmd = "expire " + key + " " + std::to_string(ttl);
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, setcmd.c_str());
        if (!r) {
            errmsg = ctx_->errstr;
            err = ctx_->err;    
            lock.unlock();
            return false;
        } else if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str, "OK") == 0)) {
            errmsg = ctx_->errstr;
            err = ctx_->err;    
            freeReplyObject(r);
            lock.unlock();
            return false;
        }
        freeReplyObject(r);
        r = (redisReply *)redisCommand(ctx_, ttlcmd.c_str());
        if (!r) {
            errmsg = ctx_->errstr;
            err = ctx_->err;    
            lock.unlock();
            return false;
        }
        freeReplyObject(r);
    }

    return true;
}

bool RedisCli::Exists (const std::string &key, int &err, std::string &errmsg) {
    err = 0;
    errmsg = "";
    std::string cmd = "exists " + key;

    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_INTEGER) {
            errmsg = ctx_->errstr;
            err = ctx_->err;
            freeReplyObject(r);
            lock.unlock();
            return false;
        }
        if (r->integer == 0) {
            lock.unlock();
            return false;
        }
    }

    return true;
}

bool RedisCli::Get (const std::string &key, std::string &value,
                    int &err, std::string &errmsg) {
    err = 0;
    errmsg = "";
    std::string cmd = "get " + key;
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_STRING) {
            if (!r || (r && r->type != REDIS_REPLY_NIL)) {
                errmsg = ctx_->errstr;
                err = ctx_->err;
            }
            freeReplyObject(r);
            lock.unlock();
            return false;
        }

        value = r->str;
        freeReplyObject(r);
    }

    return true;
}





};
