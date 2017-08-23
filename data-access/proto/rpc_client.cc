#include "pull_rpc_client.h"
#include "push_rpc_client.h"
#include "manage_rpc_client.h"

namespace RpcAdaptClient {
    namespace PullDataRpcClient {
        sofa::pbrpc::RpcChannelOptions RpcClientBase::rpc_channel_options_;
        sofa::pbrpc::RpcClientOptions RpcClientBase::rpc_client_options_;
        sofa::pbrpc::RpcClient RpcClientBase::rpc_client_;
        std::map<std::string, sofa::pbrpc::RpcChannel *> RpcClientBase::records_;
        std::mutex RpcClientBase::mutex_;
    };

    namespace PushDataRpcClient {
        sofa::pbrpc::RpcChannelOptions RpcClientBase::rpc_channel_options_;
        sofa::pbrpc::RpcClientOptions RpcClientBase::rpc_client_options_;
        sofa::pbrpc::RpcClient RpcClientBase::rpc_client_;
        std::map<std::string, sofa::pbrpc::RpcChannel *> RpcClientBase::records_;
        std::mutex RpcClientBase::mutex_;
    };

    namespace ManageRpcClient {
        sofa::pbrpc::RpcChannelOptions RpcClientBase::rpc_channel_options_;
        sofa::pbrpc::RpcClientOptions RpcClientBase::rpc_client_options_;
        sofa::pbrpc::RpcClient RpcClientBase::rpc_client_;
        std::map<std::string, sofa::pbrpc::RpcChannel *> RpcClientBase::records_;
        std::mutex RpcClientBase::mutex_;
    };
};
