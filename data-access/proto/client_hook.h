#ifndef DATA_ACCESS_MINOS_CLIENT_HOOK_H
#define DATA_ACCESS_MINOS_CLIENT_HOOK_H
#include "minos.pb.h"
#include "transmit.pb.h"
#include "manage.pb.h"
#include "pull_rpc_client.h"
#include "push_rpc_client.h"
#include "manage_rpc_client.h"

namespace RpcAdaptClient {
    namespace PushDataRpcClient {
        class PushClientHook : public RpcClient<process::ProcessService::Stub> {
        public:
            PushClientHook(const std::string &server_addr, int rpc_timeout): RpcClient<process::ProcessService::Stub>(server_addr), rpc_timeout_(rpc_timeout) {}
            virtual ~PushClientHook(){}

            bool Transmit(const process::TransmitRequest *request, process::TransmitResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const process::TransmitRequest *, process::TransmitResponse *)> cb = NULL) {
                return SendRequest(&process::ProcessService::Stub::Transmit,
                                   request,
                                   response,
                                   cb,
                                   "Transmit",
                                   rpc_timeout_);
            }
            bool SetCheckListConfig (const process::SetCheckListConfigRequest *request, process::SetCheckListConfigResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const process::SetCheckListConfigRequest *, process::SetCheckListConfigResponse *)> cb = NULL) {
                return SendRequest(&process::ProcessService::Stub::SetCheckListConfig,
                                   request,
                                   response,
                                   cb,
                                   "SetCheckListConfig",
                                   rpc_timeout_);
            }
            bool GetCheckListConfig (const process::GetCheckListConfigRequest *request, process::GetCheckListConfigResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const process::GetCheckListConfigRequest *, process::GetCheckListConfigResponse *)> cb = NULL) {
                return SendRequest(&process::ProcessService::Stub::GetCheckListConfig,
                                   request,
                                   response,
                                   cb,
                                   "GetCheckListConfig",
                                   rpc_timeout_);
            }
            bool GetLogProcessStat (const process::GetLogProcessStatRequest *request, process::GetLogProcessStatResponse *response, std::function<void (sofa::pbrpc::RpcController *cntl, const process::GetLogProcessStatRequest *, process::GetLogProcessStatResponse *)> cb = NULL) {
                return SendRequest(&process::ProcessService::Stub::GetLogProcessStat,
                                   request,
                                   response,
                                   cb,
                                   "GetLogProcessStat",
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

    namespace ManageRpcClient {
        class ManageClientHook : public RpcClient<manage::ManageService::Stub> {
        public:
            ManageClientHook(const std::string &server_addr, int rpc_timeout): RpcClient<manage::ManageService::Stub>(server_addr), rpc_timeout_(rpc_timeout) {}
            virtual ~ManageClientHook(){}

            bool SetLogFilterConfig(const manage::SetLogFilterConfigRequest *request,
                                    manage::SetLogFilterConfigResponse *response,
                                    std::function<void (sofa::pbrpc::RpcController *cntl, const manage::SetLogFilterConfigRequest *, manage::SetLogFilterConfigResponse *)> cb = NULL) {
                return SendRequest(&manage::ManageService::Stub::SetLogFilterConfig,
                                   request,
                                   response,
                                   cb,
                                   "SetLogFilterConfig",
                                   rpc_timeout_);
            }

            bool GetLogFilterConfig (const manage::GetLogFilterConfigRequest *request,
                                     manage::GetLogFilterConfigResponse *response,
                                     std::function<void (sofa::pbrpc::RpcController *cntl, const manage::GetLogFilterConfigRequest *, manage::GetLogFilterConfigResponse *)> cb = NULL) {
                return SendRequest(&manage::ManageService::Stub::GetLogFilterConfig,
                                   request,
                                   response,
                                   cb,
                                   "GetLogFilterConfig",
                                   rpc_timeout_);
            }

            bool SetLogFieldConfig (const manage::SetLogFieldConfigRequest *request,
                                    manage::SetLogFieldConfigResponse *response,
                                    std::function<void (sofa::pbrpc::RpcController *cntl, const manage::SetLogFieldConfigRequest *, manage::SetLogFieldConfigResponse *)> cb = NULL) {
                return SendRequest(&manage::ManageService::Stub::SetLogFieldConfig, 
                                   request,
                                   response,
                                   cb,
                                   "SetLogFieldConfig",
                                   rpc_timeout_);
            }

            bool GetLogFieldConfig (const manage::GetLogFieldConfigRequest *request,
                                    manage::GetLogFieldConfigResponse *response,
                                    std::function<void (sofa::pbrpc::RpcController *cntl, const manage::GetLogFieldConfigRequest *, manage::GetLogFieldConfigResponse *)> cb = NULL) {
                return SendRequest(&manage::ManageService::Stub::GetLogFieldConfig,
                                   request,
                                   response,
                                   cb,
                                   "GetLogFieldConfig",
                                   rpc_timeout_);
            }

            bool GetLogTransmitStat (const manage::GetLogTransmitStatRequest *request,
                                     manage::GetLogTransmitStatResponse *response,
                                     std::function<void (sofa::pbrpc::RpcController *cntl, const manage::GetLogTransmitStatRequest *, manage::GetLogTransmitStatResponse *)> cb = NULL) {
                return SendRequest(&manage::ManageService::Stub::GetLogTransmitStat,
                                   request,
                                   response,
                                   cb,
                                   "GetLogTransmitStat",
                                   rpc_timeout_);
            }
            
            bool PullSwitchConfig (const manage::PullSwitchConfigRequest *request,
                                   manage::PullSwitchConfigResponse *response,
                                   std::function<void (sofa::pbrpc::RpcController *cntl, const manage::PullSwitchConfigRequest *, manage::PullSwitchConfigResponse *)> cb = NULL) {

                return SendRequest(&manage::ManageService::Stub::PullSwitchConfig,
                                   request,
                                   response,
                                   cb,
                                   "PullSwitchConfig",
                                   rpc_timeout_);
            }

            bool Retransmit (const manage::RetransmitRequest *request,
                             manage::RetransmitResponse *response,
                             std::function<void (sofa::pbrpc::RpcController *cntl, const manage::RetransmitRequest *, manage::RetransmitResponse *)> cb = NULL) {
                
                return SendRequest(&manage::ManageService::Stub::Retransmit,
                                   request,
                                   response,
                                   cb,
                                   "Retransmit",
                                   rpc_timeout_);
            }
        private:
            int rpc_timeout_;
        };
    };
};
#endif
