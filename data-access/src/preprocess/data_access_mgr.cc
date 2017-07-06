#include "data_access_mgr.h"

DEFINE_string(BROKER_SERVER, "127.0.0.1:425", "minos agent serveraddr is 127.0.0.1:425 in all baidu machines");
DEFINE_int32(REQUEST_TIMEOUT, 3000, "rpc request to minos broker timeout is default as 3 seconds");
DEFINE_int32(LOCKFREEQUEUE_LEN, 100000000, "lockfreequeue of dataaccess");
DEFINE_int32(PULLERS_LEN, 20, "means that how many threads(partition) to pull log from minos, defined by minos-config on minos platform");

namespace dataaccess {
    
LFQ lockfreequeue_;;

static void InitTransmitQueue (int64_t len) {
    lockfreequeue_.reset(new boost::lockfree::queue<void *>(len));
}

static void *ThreadFunc (void *arg) {
    DataAccesser *accesser = reinterpret_cast<DataAccesser *>(arg);
    accesser->Subscribe(lockfreequeue_);
    return NULL;
}

DataAccesserMgr::DataAccesserMgr (const libconfig::Setting &cfg) {
    q_len_ = ParseQueueLen(cfg);
    InitTransmitQueue(q_len_);
    
    pullers_num_ = ParsePullersNum(cfg);
    ParseBrokerServerAddr(cfg);
    ParseRpcTimeout(cfg);
    if (!Start(cfg)) {
        for (auto i: common::Range(0, accessers_.size())) {
            DataAccesser *accesser = accessers_[i];
            delete accesser;
            accesser = accessers_[i] = 0;
        }
        accessers_.clear();
    }
}

DataAccesserMgr::~DataAccesserMgr() {
    Stop();
}

bool DataAccesserMgr::Start(const libconfig::Setting &minos_cfg) {
    libconfig::Setting &cfg = minos_cfg["accessers"];
    //for (auto i: common::Range(0, pullers_num_)) {
    for (auto i: common::Range(0, 1)) {
        DataAccesser *accesser = new DataAccesser(broker_server_addr_, rpc_timeout_, cfg[i]);
        if (!accesser || "" != accesser->GetError()) {
            if (!accesser) {
                LOG(ERROR) << "accesser construct fail";
            } else {
                LOG(ERROR) << "accesser init fail";
            }
            return false;
        }
        LOG(INFO) << "construct DataAccesser " << i << " success";
        accessers_.push_back(accesser);
    }

    //for (auto i: common::Range(0, pullers_num_)) {
    for (auto i: common::Range(0, 1)) {
        DataAccesser *accesser = accessers_[i];
        pullers_.push_back(std::thread(&ThreadFunc, accesser));
    }
    return true;
}

void DataAccesserMgr::Stop () {
    for (auto i: common::Range(0, pullers_.size())) {
        pthread_t tid = pullers_[i].native_handle();
        pthread_kill(tid, SIGTERM);
    }

    for (auto i: common::Range(0, accessers_.size())) {
        DataAccesser *accesser = accessers_[i];
        delete accesser;
        accesser = accessers_[i] = 0;
    }
    accessers_.clear();
}

void DataAccesserMgr::ParseBrokerServerAddr (const libconfig::Setting &cfg) {
    cfg.lookupValue("broker_server_addr", broker_server_addr_);
    if (broker_server_addr_ == "") {
        broker_server_addr_ = FLAGS_BROKER_SERVER;
    }
}

void DataAccesserMgr::ParseRpcTimeout (const libconfig::Setting &cfg) {
    rpc_timeout_ = FLAGS_REQUEST_TIMEOUT;
    cfg.lookupValue("rpc_timeout", rpc_timeout_);
    if (rpc_timeout_ <= 0) {
        rpc_timeout_ = FLAGS_REQUEST_TIMEOUT;
    }
}

int DataAccesserMgr::ParseQueueLen(const libconfig::Setting &cfg) {
    int q_len = FLAGS_LOCKFREEQUEUE_LEN;
    cfg.lookupValue("queue_len", q_len);
    if (q_len <= 0) {
        q_len = FLAGS_LOCKFREEQUEUE_LEN;
    }
    return q_len;
}

int DataAccesserMgr::ParsePullersNum (const libconfig::Setting &cfg) {
    int pullers_len = FLAGS_PULLERS_LEN;
    cfg.lookupValue("pullers_len", pullers_len);
    if (pullers_len <= 0) {
        pullers_len = FLAGS_PULLERS_LEN;
    }
    return pullers_len;
}









};
