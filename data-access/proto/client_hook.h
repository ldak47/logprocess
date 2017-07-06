#ifndef DATA_ACCESS_MINOS_CLIENT_HOOK_H
#define DATA_ACCESS_MINOS_CLIENT_HOOK_H
#include "minos.pb.h"
#include "transmit.pb.h"
#include "pull_rpc_client.h"
#include "push_rpc_client.h"

namespace RpcAdaptClient {
    namespace PushDataRpcClient {
        class PushClientHook : public RpcClient<transmit::TransmitService::Stub> {
        public:
            PushClientHook(const std::string &server_addr, int rpc_timeout): RpcClient<transmit::TransmitService::Stub>(server_addr), rpc_timeout_(rpc_timeout) {}
            virtual ~PushClientHook(){}

            bool Transmit(const transmit::TransmitRequest *request, transmit::TransmitResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const transmit::TransmitRequest *, transmit::TransmitResponse *)> cb = NULL) {
                return SendRequest(&transmit::TransmitService::Stub::transmit,
                                   request,
                                   response,
                                   cb,
                                   "Transmit",
                                   rpc_timeout_);
            }
        private:
            int rpc_timeout_;
        };
    };
    
    namespace PullDataRpcClient {
        class PullClientHook : public RpcClient<baidu::minos::BrokerService::Stub> {
        public:
            PullClientHook(const std::string &server_addr, int rpc_timeout): RpcClient<baidu::minos::BrokerService::Stub>(server_addr), rpc_timeout_(rpc_timeout) {}
            virtual ~PullClientHook(){}
            
            bool InitSub(const baidu::minos::InitSubscriberBrokerRequest *request, baidu::minos::InitSubscriberBrokerResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const baidu::minos::InitSubscriberBrokerRequest *, baidu::minos::InitSubscriberBrokerResponse *)> cb = NULL) {
                return SendRequest(&baidu::minos::BrokerService::Stub::init_subscriber,
                           request,
                           response,
                           cb,
                           "InitSub",
                           rpc_timeout_);
            }
            bool UninitSub(const baidu::minos::UninitSubscriberBrokerRequest *request, baidu::minos::BrokerResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const baidu::minos::UninitSubscriberBrokerRequest *, baidu::minos::BrokerResponse *)> cb = NULL) {
                return SendRequest(&baidu::minos::BrokerService::Stub::uninit_subscriber,
                           request,
                           response,
                           cb,
                           "UninitSub",
                           rpc_timeout_);
            }
            bool GetPartitionProgress(const baidu::minos::GetPartitionGroupProgressRequest *request, baidu::minos::GetPartitionGroupProgressResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const baidu::minos::GetPartitionGroupProgressRequest *, baidu::minos::GetPartitionGroupProgressResponse *)> cb = NULL) {
                return SendRequest(&baidu::minos::BrokerService::Stub::get_partition_group_progress,
                           request,
                           response,
                           cb,
                           "GetPartitionProgress",
                           rpc_timeout_);
            }
            bool SetPartitionProgress(const baidu::minos::SetPartitionGroupProgressRequest *request, baidu::minos::BrokerResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const baidu::minos::SetPartitionGroupProgressRequest *, baidu::minos::BrokerResponse *)> cb = NULL) {
                return SendRequest(&baidu::minos::BrokerService::Stub::set_partition_group_progress,
                           request,
                           response,
                           cb,
                           "SetPartitionProgress",
                           rpc_timeout_);
            }
            bool Subscribe(const baidu::minos::SubscribeRecordRequest *request, baidu::minos::SubscribeRecordResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const baidu::minos::SubscribeRecordRequest *, baidu::minos::SubscribeRecordResponse *)> cb = NULL) {
                return SendRequest(&baidu::minos::BrokerService::Stub::subscribe_record,
                           request,
                           response,
                           cb,
                           "Subscribe",
                           rpc_timeout_);
            }

        private:
            int rpc_timeout_;
        };
    };
};
#endif
