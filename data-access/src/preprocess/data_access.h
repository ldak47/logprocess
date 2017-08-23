#include "util.h"
#include "client_hook.h"
#include "data_util.h"
#include "range_iterator.h"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "libconfig.h++"
#include <string>
#include <regex.h>
#include <boost/lockfree/queue.hpp>
#include <boost/algorithm/string.hpp>
#include "data_access_filter.h"
#include "timer.h"
#include "data_access_pv.h"
#include "data_preprocess_action.h"
#include "health.h"
#include "curl.h"
#include "cJSON/cJSON.h"
#include "thread_pool.h"

namespace dataaccess {
    
enum DataAccesserStatus {
    INITING = 0,
    RUNNING = 1,
    PAUSING = 2,
};

using LFQ = std::shared_ptr<boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>>;

class DataAccesser {
public:
    DataAccesser(std::string server_addr,
                 int rpc_timeout,
                 const libconfig::Setting &minos_cfg,
                 const std::vector<std::string> &support_type,
                 const libconfig::Setting &srcids_cfg,
                 AccessFilter &datafilter,
                 int topic_id,
                 std::string token,
                 bool type,
                 bool &pullswitch,
                 PvStater &pvstater,
                 DataFieldRuler &ruler,
                 std::string &platformaddr
                 );
    ~DataAccesser();
    bool Start(const libconfig::Setting &minos_cfg,
               const std::vector<std::string> &support_type,
               const libconfig::Setting &sricds_cfg);
    bool Stop();
    void Subscribe();
    void InitMinosPipe();
    std::string GetError () const {
        return failure_str;
    }
    
    bool filter(const std::string &msg);
    
private:
    std::string server_addr_;
    int rpc_timeout_;
    std::unique_ptr<RpcAdaptClient::PullDataRpcClient::PullClientHook> client_;
    
    std::unique_ptr<ThreadPool> resender;
    bool GetRelaxingNode(std::string &freenode);
    
    bool minos_arg_parse(const libconfig::Setting &minos_cfg);
    int topic_id_;
    std::string token_;
    int partition_;
    int64_t default_package_id_;
    int default_record_count_;

    AccessFilter &datafilter_;

    DataAccesserStatus status_;
    uint64_t access_cnt_;

    std::string failure_str;
    bool type_;
    bool &pullswitch_;

    std::string Get_Timestamp(const std::string &body);
    std::string Get_Srcid(const std::string &body);
    bool Get_Resno(const std::string &body);
    std::string Get_Application(const std::string &body);
    PvStater &pvstater_;
    DataFieldRuler &ruler_;
    std::string &platformaddr_;
};

};
