#ifndef PROCESS_ADAPT_H
#define PROCESS_ADAPT_H
#include "client_hook.h"
#include "process_impl.h"

namespace dataprocess {

    class ProcessAdapt : public transmit::TransmitService {
        std::unique_ptr<ThreadPool> thread_pool_;
        Process_Impl process_impl_;
        bool err_flag_;
        std::string server_addr_;
        std::unique_ptr<sofa::pbrpc::RpcServer> rpc_server_;
        
        bool Init(const libconfig::Setting &cfg);
    public:
        ProcessAdapt(const libconfig::Setting &cfg);
        ~ProcessAdapt();
        bool Start(const libconfig::Setting &cfg);
        bool GetErrFlag(){return err_flag_;}

        void transmit(::google::protobuf::RpcController *cntl,
                         const transmit::TransmitRequest *request,
                         transmit::TransmitResponse *response,
                         ::google::protobuf::Closure *done);

        void DataReceive_action(const transmit::TransmitRequest *request,
                                transmit::TransmitResponse *response,
                                ::google::protobuf::Closure *done); 
    };
};

#endif
