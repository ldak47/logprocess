#ifndef PROCESS_ADAPT_H
#define PROCESS_ADAPT_H
#include "client_hook.h"
#include "process_impl.h"

namespace dataprocess {

    class ProcessAdapt : public process::ProcessService {
        std::unique_ptr<ThreadPool> thread_pool_;
        std::unique_ptr<Process_Impl> process_impl_;
        bool err_flag_;
        std::string server_addr_;
        std::unique_ptr<sofa::pbrpc::RpcServer> rpc_server_;
        
        bool Init(const libconfig::Setting &cfg, 
                  const libconfig::Setting &rdscfg, 
                  const std::vector<std::string> &types, 
                  int port,
                  const std::string &accessid,
                  const std::string &dbpath,
                  const std::string &plathttpaddr);
    public:
        ProcessAdapt(const libconfig::Setting &cfg, 
                     const libconfig::Setting &rdscfg, 
                     const std::string &accessid, 
                     const std::string &server_addr,
                     const std::string &port, 
                     const std::string &dbpath, 
                     const std::vector<std::string> &types,
                     const std::string &plathttpaddr);
        ~ProcessAdapt();
        bool Start(const libconfig::Setting &cfg, const std::string &port, const std::string &accessid, const std::string &dbpath);
        bool GetErrFlag(){return err_flag_;}

        void Transmit(::google::protobuf::RpcController *cntl,
                         const process::TransmitRequest *request,
                         process::TransmitResponse *response,
                         ::google::protobuf::Closure *done);

        void DataReceive_action(const process::TransmitRequest *request,
                                process::TransmitResponse *response,
                                ::google::protobuf::Closure *done); 

        void SetCheckListConfig (::google::protobuf::RpcController *cntl,
                                 const process::SetCheckListConfigRequest *request,
                                 process::SetCheckListConfigResponse *response,
                                 ::google::protobuf::Closure *done);

        void GetCheckListConfig (::google::protobuf::RpcController *cntl,
                                 const process::GetCheckListConfigRequest *request,
                                 process::GetCheckListConfigResponse *response,
                                 ::google::protobuf::Closure *done);
        
        void GetLogProcessStat (::google::protobuf::RpcController *cntl,
                                const process::GetLogProcessStatRequest *request,
                                process::GetLogProcessStatResponse *response,
                                ::google::protobuf::Closure *done);
    };
};

#endif
