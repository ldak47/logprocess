#include "thread_pool.h"
#include "data_preprocess_action.h"
#include "libconfig.h++"
#include "client_hook.h"
#include "redis.h"
#include "consistent_hash.h"
#include "boost/lockfree/queue.hpp"
#include <regex.h>
#include <random>
#include "data_access_pv.h"
#include "data_access_filter.h"
#include "timer.h"

namespace dataaccess {

class DataPreProcess {
    int worker_num_;
    std::unique_ptr<ThreadPool> workers_;
    std::unique_ptr<std::thread> agent_;
    std::unique_ptr<std::thread> agent_new_;
    
    std::string failure_;
    bool stop_;
    std::mutex mtx_;
    std::condition_variable cond_;

    DataFieldRuler &ruler_;
    std::map<std::string, std::string> FieldParse();

    std::unique_ptr<common::RedisCli> redisctx_;
    int delaytask_worker_num_;
    std::unique_ptr<ThreadPool> delaytask_workers_;
    void DelayJudgeForward();

    std::unique_ptr<common::ConsistentHash> loadbalancer_;
    std::string GetProcessSerer(std::string query);

    bool GetSrcid(const std::string &body, std::string &srcid);
    std::map<std::string, std::string> GetFields(const std::string &body, const std::map<std::string, std::string> &field_regexes, bool type);
    void kv2request(const std::map<std::string, std::string> &kv, 
                    process::TransmitRequest &request,
                    const std::map<std::string, bool> &istrans);

    void Transmit(const std::map<std::string, std::string> &kv, 
                  const std::map<std::string, bool> &istrans);

    void PreProcess(void *msg);
    void PreProcess_new(void *msg);
    
public:
    DataPreProcess(const libconfig::Setting &cfg, 
                   DataFieldRuler &ruler, 
                   const std::string &rdsaddr, 
                   int rdsport,
                   const std::vector<std::string> &process_server_cfg);
    ~DataPreProcess();
    bool Start();
    bool Stop(bool graceful);
};
    
};
