#include "data_access.h"

namespace dataaccess {

class DataAccesserMgr {
    std::thread puller;
    std::thread puller_new;
    
    int q_len_;
    
    std::unique_ptr<DataAccesser> accesser_;
    std::unique_ptr<DataAccesser> accesser_new_;
    int access_id_;
    void ParseAccessid(const libconfig::Setting &cfg);

    int ParseQueueLen(const libconfig::Setting &cfg);
    void ParseBrokerServerAddr(const libconfig::Setting &cfg);
    void ParseRpcTimeout(const libconfig::Setting &cfg);
    void ParseTopic_Token(const libconfig::Setting &cfg,
                       int &old_topic,
                       int &new_topic,
                       std::string &old_token,
                       std::string &new_token
                       );
    std::string broker_server_addr_;
    int rpc_timeout_;
    
    bool err_;

public:
    DataAccesserMgr(const libconfig::Setting &cfg, 
                    const libconfig::Setting &minos_new_cfg,
                    const std::vector<std::string> &support_type, 
                    const libconfig::Setting &srcids_cfg,
                    AccessFilter &datafilter,
                    bool &switch_old,
                    bool &switch_new,
                    int accessid,
                    const std::string &pullserver,
                    PvStater &pvstater,
                    DataFieldRuler &ruler,
                    std::string &platformaddr
                    );
    ~DataAccesserMgr();
    bool Start(const libconfig::Setting &minos_cfg, 
               const libconfig::Setting &minos_new_cfg,
               const std::vector<std::string> &support_type, 
               const libconfig::Setting &srcids_cfg,
               AccessFilter &datafilter,
               bool &switch_old,
               bool &switch_new,
               const std::string &pullserver,
               PvStater &pvstater,
               DataFieldRuler &ruler,
               std::string &platformaddr
               );
    void Stop();
    bool GetError() {
        return err_;
    }
};







};
