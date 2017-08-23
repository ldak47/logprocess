#ifndef DATAACCESS_PROCESS_STAT_H
#define DATAACCESS_PROCESS_STAT_H
#include <mutex>
#include <memory>
#include <vector>
#include <map>
#include "process_stat_query_freq.h"
#include "redis.h"

namespace dataprocess {

class ProcessStater {
    std::mutex mtx;
    std::unique_ptr<common::RedisCli> rdb_;
    std::map<std::string, std::function<void (std::map<std::string, std::string>)>> staters;
    std::string err_;
    
public:
    ProcessStater(const libconfig::Setting &rdscfg):err_("") {
        try {
            rdb_.reset(new common::RedisCli(rdscfg));
            if (!rdb_.get()) {
                err_ = "rdb_ reset fail";
            }
        } catch (std::exception &err) {
            err_ = err.what();
        }
    }
    ~ProcessStater(){
        staters.clear();
    }
    std::string GetError(){return err_;}
    
    std::vector<std::string> GetStaters();
    void AddStater(const std::string notice, std::function<void (const std::map<std::string, std::string> &)> stater);
    void DelStater(const std::string notice);
    void Run(const std::map<std::string, std::string> &kv);
};

};
#endif
