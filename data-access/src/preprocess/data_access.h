#include "util.h"
#include "client_hook.h"
#include "data_util.h"
#include "range_iterator.h"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "libconfig.h++"
#include <string>
#include <boost/lockfree/queue.hpp>

namespace dataaccess {
    
enum DataAccesserStatus {
    INITING = 0,
    RUNNING = 1,
    PAUSING = 2,
};
using LFQ = std::shared_ptr<boost::lockfree::queue<void *>>;

class DataAccesser {
public:
    DataAccesser(std::string server_addr,
                 int rpc_timeout,
                 const libconfig::Setting &minos_cfg
                 );
    ~DataAccesser();
    bool Start(const libconfig::Setting &minos_cfg);
    bool Stop();
    void Subscribe(std::shared_ptr<boost::lockfree::queue<void *>> lockfreequeue);
    void InitMinosPipe();
    std::string GetError () const {
        return failure_str;
    }
private:
    std::string server_addr_;
    int rpc_timeout_;
    std::unique_ptr<RpcAdaptClient::PullDataRpcClient::PullClientHook> client_;
    
    bool minos_arg_parse(const libconfig::Setting &minos_cfg);
    int topic_id_;
    std::string token_;
    int partition_;
    int64_t default_package_id_;
    int default_record_count_;

    DataAccesserStatus status_;
    uint64_t access_cnt_;
    uint64_t error_cnt_;

    std::string failure_str;
};

};
