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
        
        bool Init(const libconfig::Setting &cfg, const std::vector<std::string> &types, int port);
    public:
        ProcessAdapt(const libconfig::Setting &cfg, const std::string &port, const std::vector<std::string> &types);
        ~ProcessAdapt();
        bool Start(const libconfig::Setting &cfg, const std::string &port);
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
