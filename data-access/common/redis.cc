#include "redis.h"
#include "glog/logging.h"


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

RedisCli::RedisCli (std::string ipaddr, int port) {
    ip_ = ipaddr;
    port_ = port;
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

bool RedisCli::Incr (const std::string &key) {
    std::string cmd = "incr " + key;

    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_INTEGER) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;
            if (r) {
                freeReplyObject(r);
            }
            lock.unlock();
            return false;
        }
        freeReplyObject(r);
    }

    return true;
}

bool RedisCli::Set (const std::string &key, const std::string &value, int ttl) {
    if (default_ttl_ > 0) {
        ttl = default_ttl_;
    }
    std::string setcmd = "set " + key + " " + value;
    std::string ttlcmd = "expire " + key + " " + std::to_string(ttl);
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, setcmd.c_str());
        if (!r) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;    
            lock.unlock();
            return false;
        } else if (!(r->type == REDIS_REPLY_STATUS && strcasecmp(r->str, "OK") == 0)) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;    
            freeReplyObject(r);
            lock.unlock();
            return false;
        }
        freeReplyObject(r);
        if (ttl > 0) {
            r = (redisReply *)redisCommand(ctx_, ttlcmd.c_str());
            if (!r) {
                errmsg_ = ctx_->errstr;
                err_ = ctx_->err;    
                lock.unlock();
                return false;
            }
            freeReplyObject(r);
        }
    }

    return true;
}

bool RedisCli::Exists (const std::string &key) {
    std::string cmd = "exists " + key;

    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_INTEGER) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;
            freeReplyObject(r);
            lock.unlock();
            return false;
        }
        if (r->integer == 0) {
            lock.unlock();
            return false;
        }
        freeReplyObject(r);
    }

    return true;
}

bool RedisCli::Get (const std::string &key, std::string &value) {
    std::string cmd = "get " + key;
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_STRING) {
            if (!r || (r && r->type != REDIS_REPLY_NIL)) {
                errmsg_ = ctx_->errstr;
                err_ = ctx_->err;
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

bool RedisCli::Zset (const std::string &key, const std::string &value, int64_t score) {
    char buf[1024] = {0};
    sprintf(buf, "zadd %s %lld %s", key.c_str(), score, value.c_str());
    std::string cmd = buf;
    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r) {
            errmsg_ = ctx_->errstr;
    LOG(INFO) << "!r: " << ctx_->err;
            err_ = ctx_->err;    
            lock.unlock();
            return false;
        } else if (r->type != REDIS_REPLY_INTEGER) {
            errmsg_ = ctx_->errstr;
    LOG(INFO) << "REDIS_REPLY_INTEGER: " << ctx_->err;
            err_ = ctx_->err;    
            freeReplyObject(r);
            lock.unlock();
            return false;
        }
        freeReplyObject(r);
    }
 
    return true;   
}

bool RedisCli::Zcount (const std::string &key, uint64_t &count, int64_t start_score, int64_t end_score) {
    std::string cmd;
    if (start_score == 0 && end_score == -1) {
        cmd = "zcount " + key + " -inf +inf";
    } else {
        cmd = "zcount " + key + " " + std::to_string(start_score) + " " + std::to_string(end_score); 
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_INTEGER) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;
            if (r) {
                freeReplyObject(r);
            }
            lock.unlock();
            return false;
        }

        count = r->integer;
        freeReplyObject(r);
    }

    return true;
}

bool RedisCli::Zrange (const std::string &key, std::vector<std::string> &values, int64_t start, int64_t end, bool direct) {
    std::string cmd;
    if (direct) {
        cmd = "zrange " + key + " " + std::to_string(start) + " " + std::to_string(end);
    } else {
        cmd = "zrevrange " + key + " " + std::to_string(start) + " " + std::to_string(end);
    }
    
    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_ARRAY) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;
            if (r) {
                freeReplyObject(r);
            }
            lock.unlock();
            return false;
        }

        for (int i = 0; i < r->elements; i++) {
            redisReply *childReply = r->element[i];
            if (childReply->type == REDIS_REPLY_STRING) {
                values.push_back(childReply->str);
            }
        }
        freeReplyObject(r);
    }

    return true;
}

bool RedisCli::ZrangeByScore (const std::string &key, std::vector<std::string> &values, std::string start_score, std::string end_score, bool direct) {
    std::string cmd;
    if (start_score == "") {
        start_score = "-inf";
    }
    if (end_score == "") {
        end_score = "+inf";
    }
    if (direct) {
        cmd = "ZRANGEBYSCORE " + key + " " + start_score + " " + end_score;
    } else {
        cmd = "ZREVRANGEBYSCORE " + key + " " + end_score + " " + start_score;
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_ARRAY) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;
            if (r) {
                freeReplyObject(r);
            }
            lock.unlock();
            return false;
        }

        for (int i = 0; i < r->elements; i++) {
            redisReply *childReply = r->element[i];
            if (childReply->type == REDIS_REPLY_STRING) {
                values.push_back(childReply->str);
            }
        }
        freeReplyObject(r);
    }

    return true;
}

bool RedisCli::ZrangeByScoreWithScore (const std::string &key, std::map<std::string, std::string> &values, std::string start_score, std::string end_score, bool direct) {
    std::string cmd;
    if (start_score == "") {
        start_score = "-inf";
    }
    if (end_score == "") {
        end_score = "+inf";
    }
    if (direct) {
        cmd = "ZRANGEBYSCORE " + key + " " + start_score + " " + end_score + " withscores";
    } else {
        cmd = "ZREVRANGEBYSCORE " + key + " " + end_score + " " + start_score + " withscores";
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_ARRAY) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;
            if (r) {
                freeReplyObject(r);
            }
            lock.unlock();
            return false;
        }

        for (int i = 0; i < r->elements; i += 2) {
            redisReply *childReply1 = r->element[i];
            redisReply *childReply2 = r->element[i + 1];
            std::string score = childReply2->str, content = childReply1->str;
            values.insert(make_pair(score, content));
        }
        freeReplyObject(r);
    }

    return true;
}

bool RedisCli::ZRem (const std::string &key, int64_t st_score, int64_t ed_score, bool type) {
    std::string cmd;
    if (st_score == 0 && ed_score == -1) {
        cmd = "zrem " + key + " 0 -1";
    } else {
        if (type) {
            cmd = "ZREMRANGEBYSCORE " + key + " " + std::to_string(st_score) + " " + std::to_string(ed_score);
        } else {   
            cmd = "ZREMRANGEBYRANK " + key + " " + std::to_string(st_score) + " " + std::to_string(ed_score);
        }
    }

    {
        std::unique_lock<std::mutex> lock(mtx);
        redisReply *r = (redisReply *)redisCommand(ctx_, cmd.c_str());
        if (!r || r->type != REDIS_REPLY_INTEGER) {
            errmsg_ = ctx_->errstr;
            err_ = ctx_->err;
            if (r) {
                freeReplyObject(r);
            }
            lock.unlock();
            return false;
        }
        freeReplyObject(r);
    }

    return true;
}

};
