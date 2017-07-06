#include "thread_pool.h"
#include "data_preprocess_action.h"
#include "libconfig.h++"
#include "client_hook.h"
#include <regex>

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
    
public:
    DataPreProcess(const libconfig::Setting &cfg, DataFieldRuler &ruler);
    ~DataPreProcess();
    bool Start();
    bool Stop(bool graceful);
};
    
};
