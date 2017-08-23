#include "data_access_mgr.h"

DEFINE_int32(ACCESS_ID, 0, "DEFAULT MINOS QUEUE ACCESS ID");
DEFINE_string(BROKER_SERVER, "127.0.0.1:425", "minos agent serveraddr is 127.0.0.1:425 in all baidu machines");
DEFINE_int32(REQUEST_TIMEOUT, 3000, "rpc request to minos broker timeout is default as 3 seconds");
DEFINE_int32(LOCKFREEQUEUE_LEN, 65530, "lockfreequeue of dataaccess");
DEFINE_int32(PULLERS_LEN, 20, "means that how many threads(partition) to pull log from minos, defined by minos-config on minos platform");

DEFINE_int32(OLD_TOPIC_ID, 70029159, "OLD_TOPIC_ID");
DEFINE_int32(NEW_TOPIC_ID, 70032423, "NEW_TOPIC_ID");
DEFINE_string(OLD_TOKEN, "minos", "OLD_TOKEN");
DEFINE_string(NEW_TOKEN, "minos", "NEW_TOKEN");

namespace dataaccess {
    
LFQ lockfreequeue_;
LFQ lockfreequeue_new_;

static void InitTransmitQueue (int len) {
    lockfreequeue_.reset(new boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>(len));
    lockfreequeue_new_.reset(new boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>(len));
    assert(lockfreequeue_.get());
    assert(lockfreequeue_new_.get());
}

DataAccesserMgr::DataAccesserMgr (const libconfig::Setting &minos_cfg, 
                                  const libconfig::Setting &minos_new_cfg,
                                  const std::vector<std::string> &support_type, 
                                  const libconfig::Setting &srcids_cfg,
                                  AccessFilter &datafilter,
                                  bool &switch_old,
                                  bool &switch_new,
                                  int access_id,
                                  const std::string &pullserver,
                                  PvStater &pvstater,
                                  DataFieldRuler &ruler,
                                  std::string &platformaddr): err_(false), access_id_(access_id) {
    //these configs, pae == paex
    q_len_ = ParseQueueLen(minos_cfg);
    InitTransmitQueue(q_len_);
 
    //ParseBrokerServerAddr(minos_cfg);
    broker_server_addr_ = pullserver;
    ParseRpcTimeout(minos_cfg);
    if (!Start(minos_cfg, minos_new_cfg, support_type, srcids_cfg, datafilter, switch_old, switch_new, pullserver, pvstater, ruler, platformaddr)) {
        LOG(ERROR) << "DataAccesserMgr Start fail.";
        err_ = true;
    }
}

DataAccesserMgr::~DataAccesserMgr() {
    Stop();
}

bool DataAccesserMgr::Start(const libconfig::Setting &minos_cfg, 
                            const libconfig::Setting &minos_new_cfg,
                            const std::vector<std::string> &support_type, 
                            const libconfig::Setting &srcids_cfg, 
                            AccessFilter &datafilter,
                            bool &switch_old,
                            bool &switch_new,
                            const std::string &pullserver,
                            PvStater &pvstater,
                            DataFieldRuler &ruler,
                            std::string &platformaddr) {
    libconfig::Setting &cfg = minos_cfg["accessers"];
    libconfig::Setting &new_cfg = minos_new_cfg["accessers"];

    //temply, paex's topic_id and token, is in pae config.
    int old_topic, new_topic;
    std::string old_token, new_token;
    ParseTopic_Token(minos_cfg, old_topic, new_topic, old_token, new_token);

    auto init_old = std::thread([this, &cfg, &support_type, &srcids_cfg, &datafilter, &switch_old, &pvstater, &ruler, &platformaddr, old_topic, old_token] () {
        LOG(INFO) << "DataAccesser(old) start... operator id: " << access_id_;
        accesser_.reset(new DataAccesser(broker_server_addr_, rpc_timeout_, cfg[access_id_], support_type, srcids_cfg, datafilter, old_topic, old_token, false, switch_old, pvstater, ruler, platformaddr));
        if (!accesser_.get() || "" != accesser_->GetError()) {
            if (!accesser_.get()) {
                LOG(ERROR) << "old accesser construct fail";
            } else {
                LOG(ERROR) << "old accesser init fail";
            }
            err_ = true;
        } else {
            LOG(INFO) << "construct old DataAccesser " << access_id_ << " success";
            puller = std::thread([this] () {
                while (1) {
                    accesser_->Subscribe();
                }
            });
        }
    });

    auto init_new = std::thread([this, &new_cfg, &support_type, &srcids_cfg, &datafilter, &switch_new, &pvstater, &ruler, &platformaddr, new_topic, new_token] () {
        LOG(INFO) << "DataAccesser(new) start... operator id: " << access_id_;
        accesser_new_.reset(new DataAccesser(broker_server_addr_, rpc_timeout_, new_cfg[access_id_], support_type, srcids_cfg, datafilter, new_topic, new_token, true, switch_new, pvstater, ruler, platformaddr));
        if (!accesser_new_.get() || "" != accesser_new_->GetError()) {
            if (!accesser_new_.get()) {
                LOG(ERROR) << "new accesser construct fail";
            } else {
                LOG(ERROR) << "new accesser init fail";
            }
            err_ = true;
        } else {
            LOG(INFO) << "construct new DataAccesser " << access_id_ << " success";
            puller_new = std::thread([this] () {
                while (1) {
                    accesser_new_->Subscribe();
                }
            });
        }
    });
    
    init_old.join();
    init_new.join();
    return !err_;
}

void DataAccesserMgr::Stop () {
    pthread_t tid = puller.native_handle();
    pthread_kill(tid, SIGTERM);
    puller.join();
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

void DataAccesserMgr::ParseAccessid (const libconfig::Setting &cfg) {
    cfg.lookupValue("access_id", access_id_);
    if (access_id_ < 0) {
        access_id_ = FLAGS_ACCESS_ID;
    }
}

void DataAccesserMgr::ParseTopic_Token (const libconfig::Setting &cfg,
                                        int &old_topic,
                                        int &new_topic,
                                        std::string &old_token,
                                        std::string &new_token
                                        ) {
    cfg.lookupValue("old_topic_id", old_topic);
    if (old_topic <= 0) {
        old_topic = FLAGS_OLD_TOPIC_ID;
    }
    cfg.lookupValue("new_topic_id", new_topic);
    if (new_topic <= 0) {
        new_topic = FLAGS_NEW_TOPIC_ID;
    }
    cfg.lookupValue("old_token", old_token);
    if (old_token == "") {
        old_token = FLAGS_OLD_TOKEN;
    }
    cfg.lookupValue("new_token", new_token);
    if (new_token == "") {
        new_token = FLAGS_NEW_TOKEN;
    }
}



};
