#ifndef DATAACCESS_ACCESS_ADAPT_H
#define DATAACCESS_ACCESS_ADAPT_H
#include "client_hook.h"
#include "data_access_impl.h"
#include "gflags/gflags.h"
#include "glog/logging.h"

namespace dataaccess {

class AccessAdapt : public manage::ManageService {
    bool err_flag_;
    
    std::string server_addr_;
    int port_;
    std::unique_ptr<sofa::pbrpc::RpcServer> rpc_server_;
    std::unique_ptr<Access_Impl> access_impl_;

    bool Init(PvStater &pvstater,
              DataFieldRuler &datafieldruler,
              AccessFilter &accessfilter,
              bool &switch_old,
              bool &switch_new);
    
public:
    AccessAdapt(const libconfig::Setting &server_cfg, 
                PvStater &pvstater,
                DataFieldRuler &datafieldruler,
                AccessFilter &accessfilter,
                bool &switch_old,
                bool &switch_new,
                const std::string &port,
                const std::string &rpcaddr);
                
    ~AccessAdapt(){}
    bool Start(const libconfig::Setting &access_cfg,
               const std::string &port,
               const std::string &rpcaddr);
    bool GetError(){return err_flag_;}
    std::string GetRpcServerAddr(){return server_addr_;}

    void SetLogFilterConfig(::google::protobuf::RpcController *cntl,
                            const manage::SetLogFilterConfigRequest *request,
                            manage::SetLogFilterConfigResponse *response,
                            ::google::protobuf::Closure *done);

    void GetLogFilterConfig(::google::protobuf::RpcController *cntl,
                            const manage::GetLogFilterConfigRequest *request,
                            manage::GetLogFilterConfigResponse *response,
                            ::google::protobuf::Closure *done);

    void SetLogFieldConfig(::google::protobuf::RpcController *cntl,
                           const manage::SetLogFieldConfigRequest *request,
                           manage::SetLogFieldConfigResponse *response,
                           ::google::protobuf::Closure *done);
    
    void GetLogFieldConfig(::google::protobuf::RpcController *cntl,
                           const manage::GetLogFieldConfigRequest *request,
                           manage::GetLogFieldConfigResponse *response,
                           ::google::protobuf::Closure *done);

    void GetLogTransmitStat(::google::protobuf::RpcController *cntl,
                            const manage::GetLogTransmitStatRequest *request,
                            manage::GetLogTransmitStatResponse *response,
                            ::google::protobuf::Closure *done);
    void AddLogTransmitStat(::google::protobuf::RpcController *cntl,
                            const manage::AddLogTransmitStatRequest *request,
                            manage::AddLogTransmitStatResponse *response,
                            ::google::protobuf::Closure *done);
    void PullSwitchConfig(::google::protobuf::RpcController *cntl,
                          const manage::PullSwitchConfigRequest *request,
                          manage::PullSwitchConfigResponse *response,
                          ::google::protobuf::Closure *done);
    void Retransmit(::google::protobuf::RpcController *cntl,
                    const manage::RetransmitRequest *request,
                    manage::RetransmitResponse *response,
                    ::google::protobuf::Closure *done);
private:
    int rpc_timeout_;
};


};

#endif
