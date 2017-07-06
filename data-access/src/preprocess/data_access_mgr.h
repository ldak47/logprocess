#include "data_access.h"

namespace dataaccess {

class DataAccesserMgr {
public:
    DataAccesserMgr(const libconfig::Setting &cfg);
    ~DataAccesserMgr();
    bool Start(const libconfig::Setting &minos_cfg);
    void Stop();

private:
    int ParsePullersNum(const libconfig::Setting &cfg);
    int pullers_num_;
    int q_len_;
    std::vector<DataAccesser *> accessers_;
    std::vector<std::thread> pullers_;

    int ParseQueueLen(const libconfig::Setting &cfg);

    void ParseBrokerServerAddr(const libconfig::Setting &cfg);
    void ParseRpcTimeout(const libconfig::Setting &cfg);
    std::string broker_server_addr_;
    int rpc_timeout_;
};







};
