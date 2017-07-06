#include "process_adapt.h"

DEFINE_int32(RPC_SERVER_WORKER_NUM, 40, "process rpc server worker num");
DEFINE_string(RPC_SERVER_ADDR, "127.0.0.1:20000", "process rpc server worker num");
DEFINE_int32(RPC_SERVER_MAX_INFLOW, -1, "the max input flow (in MB/s) for rpc server, -1 means no limit");
DEFINE_int32(RPC_SERVER_MAX_OUTFLOW, -1, "the max output flow (out MB/s) for rpc server, -1 means no limit");

namespace dataprocess {
    
    ProcessAdapt::ProcessAdapt (const libconfig::Setting &cfg):err_flag_(false), server_addr_(FLAGS_RPC_SERVER_ADDR)  {
        err_flag_ = !(Init(cfg));
        if (!err_flag_) {
            err_flag_ = !(Start(cfg));
        }
    }

    ProcessAdapt::~ProcessAdapt () {   
        while (!thread_pool_->IsEmpty()) {
            std::chrono::seconds duration(1);
            std::this_thread::sleep_for(duration);
        }

        if (thread_pool_.get()) {
            thread_pool_.reset(nullptr);
        }
        if (rpc_server_.get()) {
            rpc_server_.reset(nullptr);
        }
    }

    bool ProcessAdapt::Start (const libconfig::Setting &cfg) {
        try {
            cfg.lookupValue("server_addr", server_addr_);
        } catch (std::exception &err) {
            LOG(ERROR) << "parse server_addr fail with err " << err.what();
            server_addr_ = FLAGS_RPC_SERVER_ADDR;
        }
    
        if (!rpc_server_->RegisterService(this)) {
            err_flag_ = true;
            LOG(ERROR) << "register server_addr fail";
        } else if (!rpc_server_->Start(server_addr_)) {
            err_flag_ = true;
            LOG(ERROR) << "start server_addr fail";
        }

        LOG(INFO) << "rpc server " << server_addr_ << " start success.";
        return true;
    }

    bool ProcessAdapt::Init (const libconfig::Setting &cfg) {
        int workernum = FLAGS_RPC_SERVER_WORKER_NUM;
        try {
            cfg.lookupValue("worker_num", workernum);
        } catch (std::exception &err) {
            workernum = FLAGS_RPC_SERVER_WORKER_NUM;
            LOG(ERROR) << "worker_num parse fail, use glags default value, err: " << err.what();
        }

        try {
            thread_pool_.reset(new ThreadPool(workernum));
            if (!thread_pool_.get()) {
                LOG(ERROR) << "workers create fail.";
                err_flag_ = true;
                return false;
            }
        } catch (std::exception &err) {
            LOG(ERROR) << "workers create fail with err: " << err.what();
            err_flag_ = true;
            return false;
        }

        try {
            sofa::pbrpc::RpcServerOptions rpc_options;
            rpc_options.max_throughput_in = FLAGS_RPC_SERVER_MAX_INFLOW;
            rpc_options.max_throughput_out = FLAGS_RPC_SERVER_MAX_OUTFLOW;
            rpc_server_.reset(new sofa::pbrpc::RpcServer(rpc_options));
            if (!rpc_server_.get()) {
                LOG(ERROR) << "rpc_server_ create fail.";
                err_flag_ = true;
                return false;
            }
        } catch (std::exception &err) {
            LOG(ERROR) << "rpc_server_ create fail with err: " << err.what();
            err_flag_ = true;
            return false;
        }

        return true;
    }
        
    void ProcessAdapt::transmit (::google::protobuf::RpcController *cntl,
                                    const transmit::TransmitRequest *request,
                                    transmit::TransmitResponse *response,
                                    ::google::protobuf::Closure *done) {
        auto task = std::bind(&dataprocess::ProcessAdapt::DataReceive_action, this, request, response, done);
        thread_pool_->AddTask(task);
    }

    void ProcessAdapt::DataReceive_action (const transmit::TransmitRequest *request,
                                           transmit::TransmitResponse *response,
                                           ::google::protobuf::Closure *done) {
        process_impl_.DataReceive(request, response, done);
    }
};
