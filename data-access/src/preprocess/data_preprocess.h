#include "thread_pool.h"
#include "data_preprocess_action.h"
#include "libconfig.h++"
#include "client_hook.h"
#include "redis.h"
#include "consistent_hash.h"
#include "boost/lockfree/queue.hpp"
#include <regex>
#include <random>

namespace dataaccess {

class DataPreProcess {
    int worker_num_;
    std::unique_ptr<ThreadPool> workers_;
    std::unique_ptr<std::thread> agent_;
    
    std::string failure_;
    bool stop_;
    std::mutex mtx_;
    std::condition_variable cond_;

    DataFieldRuler ruler_;
    std::map<std::string, std::string> FieldParse();
    
    std::unique_ptr<common::RedisCli> redisctx_;
    int delaytask_worker_num_;
    std::unique_ptr<ThreadPool> delaytask_workers_;
    void DelayJudgeForward();

    std::unique_ptr<common::ConsistentHash> loadbalancer_;
    std::string GetProcessSerer(std::string query);

    bool GetSrcid(const std::string &body, std::string &srcid);
    std::map<std::string, std::string> GetFields(const std::string &body, const std::map<std::string, std::string> &field_regexes);
    void kv2request(const std::map<std::string, std::string> &kv, process::TransmitRequest &request);

    void Transmit(const std::map<std::string, std::string> &kv, const std::shared_ptr<boost::lockfree::queue<void *>> &lockfreequeue_, void *msg);
    
public:
    DataPreProcess(const libconfig::Setting &cfg, DataFieldRuler &ruler, const libconfig::Setting &rdscfg, const std::vector<std::string> &process_server_cfg);
    ~DataPreProcess();
    bool Start();
    bool Stop(bool graceful);
};
    
};
