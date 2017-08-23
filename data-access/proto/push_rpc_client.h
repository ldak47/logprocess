#include "rpc_client.h"

namespace RpcAdaptClient {
    namespace PushDataRpcClient {

        class RpcClientBase {
        public:
            //配置rpc-client的传输参数
            static void SetRpcClientOption (int max_throughput_in,
                                            int max_throughput_out,
                                            int max_pending_buffer_size,
                                            int work_thread_num) {
                if (max_throughput_in > 0) {
                    rpc_client_options_.max_throughput_in = max_throughput_in;
                }
                if (max_throughput_out > 0) {
                    rpc_client_options_.max_throughput_out = max_throughput_out;
                }
                if (max_pending_buffer_size > 0) {
                    rpc_client_options_.max_pending_buffer_size = max_pending_buffer_size;
                }
                if (work_thread_num > 0) {
                    rpc_client_options_.work_thread_num = work_thread_num;
                }

                rpc_client_.ResetOptions(rpc_client_options_);
            }

        protected:
            //改去连接一个新的server
            virtual void ResetClient (const std::string &server_addr) {
                while (!mutex_.try_lock());
                std::map<std::string, sofa::pbrpc::RpcChannel *>::iterator it = records_.find(server_addr);
                if (it != records_.end()) {
                    rpc_channel_ = it->second;
                } else {
                    sofa::pbrpc::RpcChannel *newchannel = new sofa::pbrpc::RpcChannel(&rpc_client_, server_addr, rpc_channel_options_);
                    if (newchannel->Init()) {
                        rpc_channel_ = records_[server_addr] = newchannel;
                    } else {
                        LOG(WARNING) << server_addr << " channel Init fail";
                        delete newchannel;
                        newchannel = rpc_channel_ = 0;
                    }
                }
                mutex_.unlock();
            }

        protected:
            sofa::pbrpc::RpcChannel *rpc_channel_;
    
            static sofa::pbrpc::RpcChannelOptions rpc_channel_options_;
            static sofa::pbrpc::RpcClientOptions rpc_client_options_;
            static sofa::pbrpc::RpcClient rpc_client_;
            static std::map<std::string, sofa::pbrpc::RpcChannel *> records_;
            static std::mutex mutex_;
            static std::condition_variable cond_;
        };//class RpcClientBase

        template<class RpcService> class RpcClient : public RpcClientBase {
        public:
            RpcClient(const std::string &server_addr) {
                ResetClient(server_addr);
            }

            virtual ~RpcClient(){}
            std::string GetServiceAddr() const {
                return server_addr_;
            }

        protected:
            virtual void ResetClient (const std::string &server_addr) {
                if (server_addr == server_addr_ && service_.get()) {
                    return;
                }

                //首先换掉rpc_channel_
                //若正常则再换掉RpcClient
                RpcClientBase::ResetClient(server_addr);
                if (!rpc_channel_) {
                    service_.reset(0);
                } else {
                    service_.reset(new RpcService(rpc_channel_));
                }
            }

            //rpc request entrance, every rpc hook could simplely register into header files(client_hook.h), and the caller need to ready for callback function.
            template<class REQ, class RESP, class CB> bool SendRequest (void(RpcService::*func)(google::protobuf::RpcController *, const REQ *, RESP *, google::protobuf::Closure *), const REQ *request, RESP *response, CB cb, const std::string &notice, int rpc_timeout) {
                if (!service_.get()) {
                    LOG(FATAL) << "rpc_client, service_ is NULL";
                    return false;
                }

                //make rpc-controller
                sofa::pbrpc::RpcController *cntl = new sofa::pbrpc::RpcController();
                cntl->SetTimeout(rpc_timeout);
                //make rpc_cb_params, used for callback.
                RPC_CB_PARAMS<REQ, RESP, CB> *params = new RPC_CB_PARAMS<REQ, RESP, CB>(cntl, request, response, notice, cb);
                //google::protobuf::Closure *done = google::protobuf::NewCallback(&RpcClient::template SuccCallback<REQ, RESP, CB>, this, params);

                //rpc request, and by default won't wait response(cb != null), response-process is in SuccCallback
                if (!cb) {
                    ((service_.get())->*func)(cntl, request, response, 0);
                    if (cntl->Failed()) {
                        LOG(WARNING) << "cntl: " << cntl->Failed() << ", errortext: " << cntl->ErrorText();
                        delete cntl;
                        delete params;
                        return false;
                    }
                    delete cntl;
                    delete params;
                    return true;
                } else {
                    //((service_.get())->*func)(cntl, request, response, done);
                    delete cntl;
                    delete params;
                    return true;
                }
            }

            //async closure callback, must process by thread pool or better.
            template<class REQ, class RESP, class CB> static void SuccCallback (RpcClient<RpcService> *client, RPC_CB_PARAMS<REQ, RESP, CB> *params) {
                sofa::pbrpc::RpcController *cntl = params->rpc_controller_;
                const REQ *request = params->request_;
                RESP *response = params->response_;
                CB cb = params->callback_;
                delete params;
                if (cb) {
                    cb(cntl, request, response);
                }
            }

        private:
            std::unique_ptr<RpcService> service_;
            std::string server_addr_;
        };//class RpcClient
    };//namespace PullDataRpcClient
};//RpcAdaptClient
