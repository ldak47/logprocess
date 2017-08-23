#include "data_access_adapt.h"

DEFINE_string(RPC_SERVER_IPADDR, "127.0.0.1", "access rpc server worker num"); 
DEFINE_int32(RPC_SERVER_PORT, 30000, "process rpc server worker port");
DEFINE_int32(RPC_SERVER_MAX_INFLOW, -1, "the max input flow (in MB/s) for rpc server, -1 means no limit");
DEFINE_int32(RPC_SERVER_MAX_OUTFLOW, -1, "the max output flow (out MB/s) for rpc server, -1 means no limit");

namespace dataaccess {

AccessAdapt::AccessAdapt (const libconfig::Setting &access_cfg, 
                          PvStater &pvstater,
                          DataFieldRuler &datafieldruler,
                          AccessFilter &accessfilter,
                          bool &switch_old,
                          bool &switch_new,
                          const std::string &port,
                          const std::string &rpcaddr):err_flag_(false), server_addr_(rpcaddr) {
    Init(pvstater, datafieldruler, accessfilter, switch_old, switch_new);
    if (!err_flag_) {
        Start(access_cfg, rpcaddr, port);
    }
}

bool AccessAdapt::Init (PvStater &pvstater,
                        DataFieldRuler &datafieldruler,
                        AccessFilter &accessfilter,
                        bool &switch_old,
                        bool &switch_new) {
    try {
        sofa::pbrpc::RpcServerOptions rpc_options;
        rpc_options.max_throughput_in = FLAGS_RPC_SERVER_MAX_INFLOW;
        rpc_options.max_throughput_out = FLAGS_RPC_SERVER_MAX_OUTFLOW;
        rpc_options.work_thread_num = 1;
        rpc_server_.reset(new sofa::pbrpc::RpcServer(rpc_options));
        if (!rpc_server_.get()) {
            LOG(ERROR) << "rpc_server_ create fail.";
            err_flag_ = true;
            return false;
        }
    } catch (std::exception &err) {
        LOG(ERROR) << "access rpc_server_ create fail with err: " << err.what();
        err_flag_ = true;
        return false;
    }

    try {
        access_impl_.reset(new Access_Impl(pvstater, datafieldruler, accessfilter, switch_old, switch_new));
        if (!access_impl_.get()) {
            LOG(ERROR) << "access_impl_ create fail.";
            err_flag_ = true;
            return false;
        }
    } catch (std::exception &err) {
        LOG(ERROR) << "access_impl_ create fail with err " << err.what();
        err_flag_ = true;
        return false;
    }

    return true;
}

bool AccessAdapt::Start (const libconfig::Setting &access_cfg, const std::string &port, const std::string &rpcaddr) {
    /*
    try {
        access_cfg.lookupValue("manage_server_addr", server_addr_);
        access_cfg.lookupValue("manage_server_port", port_);
    } catch (std::exception &err) {
        LOG(ERROR) << "parse server_addr fail with err " << err.what();
        server_addr_ = FLAGS_RPC_SERVER_IPADDR;
        port_ = FLAGS_RPC_SERVER_PORT;
    }
    
    server_addr_ += ":";
    if (port == "") {
        server_addr_ += std::to_string(port_);
    } else {
        server_addr_ += port;
    }
    */
    LOG(INFO) << "access rpcserver " << server_addr_;
    
    if (!rpc_server_->RegisterService(this)) {
        err_flag_ = true;
        LOG(ERROR) << "register server_addr fail";
        return false;
    } else if (!rpc_server_->Start(server_addr_)) {
        err_flag_ = true;
        LOG(ERROR) << "start server_addr fail";
        return false;
    }

    LOG(INFO) << "access rpc server " << server_addr_ << " start success.";
    return true;
}

void AccessAdapt::SetLogFilterConfig (::google::protobuf::RpcController *cntl,
                                      const manage::SetLogFilterConfigRequest *request,
                                      manage::SetLogFilterConfigResponse *response,
                                      ::google::protobuf::Closure *done) {
    access_impl_->SetLogFilterConfig(request, response, done);
}

void AccessAdapt::GetLogFilterConfig(::google::protobuf::RpcController *cntl,
                                     const manage::GetLogFilterConfigRequest *request,
                                     manage::GetLogFilterConfigResponse *response,
                                     ::google::protobuf::Closure *done) {
    access_impl_->GetLogFilterConfig(request, response, done);
}

void AccessAdapt::SetLogFieldConfig(::google::protobuf::RpcController *cntl,
                                    const manage::SetLogFieldConfigRequest *request, 
                                    manage::SetLogFieldConfigResponse *response,
                                    ::google::protobuf::Closure *done) {
    access_impl_->SetLogFieldConfig(request, response, done);
}

void AccessAdapt::GetLogFieldConfig (::google::protobuf::RpcController *cntl,
                                     const manage::GetLogFieldConfigRequest *request,
                                     manage::GetLogFieldConfigResponse *response,
                                     ::google::protobuf::Closure *done) {
    access_impl_->GetLogFieldConfig(request, response, done);
}

void AccessAdapt::GetLogTransmitStat (::google::protobuf::RpcController *cntl,
                                      const manage::GetLogTransmitStatRequest *request,
                                      manage::GetLogTransmitStatResponse *response,
                                      ::google::protobuf::Closure *done) {
    access_impl_->GetLogTransmitStat(request, response, done);
}

void AccessAdapt::AddLogTransmitStat (::google::protobuf::RpcController *cntl,
                                      const manage::AddLogTransmitStatRequest *request,
                                      manage::AddLogTransmitStatResponse *response,
                                      ::google::protobuf::Closure *done) {
    access_impl_->AddLogTransmitStat(request, response, done);
}

void AccessAdapt::PullSwitchConfig (::google::protobuf::RpcController *cntl,
                                    const manage::PullSwitchConfigRequest *request,
                                    manage::PullSwitchConfigResponse *response,
                                    ::google::protobuf::Closure *done) {
    access_impl_->PullSwitchConfig(request, response, done);
}

void AccessAdapt::Retransmit (::google::protobuf::RpcController *cntl,
                              const manage::RetransmitRequest *request,
                              manage::RetransmitResponse *response,
                              ::google::protobuf::Closure *done) {
    access_impl_->Retransmit(request, response, done);
}

};
