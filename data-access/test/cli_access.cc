#include "client_hook.h"
#include <string>


bool OpLegalSrcid (bool op, const std::string &srcid) {
    std::unique_ptr<RpcAdaptClient::ManageRpcClient::ManageClientHook> client;
    client.reset(new RpcAdaptClient::ManageRpcClient::ManageClientHook("127.0.0.1:30000", 100000));
    
    manage::SetLogFilterConfigRequest request;
    manage::SetLogFilterConfigResponse response;
    request.set_op(op);
    request.set_srcid(srcid);

    bool res = client->SetLogFilterConfig(&request, &response, 0);
    LOG(INFO) << response.status();
    return res;
}

std::vector<std::string> GetLegalSrcids () {
    std::vector<std::string> srcids;
    std::unique_ptr<RpcAdaptClient::ManageRpcClient::ManageClientHook> client;
    client.reset(new RpcAdaptClient::ManageRpcClient::ManageClientHook("127.0.0.1:30000", 100000));
    
    manage::GetLogFilterConfigRequest request;
    manage::GetLogFilterConfigResponse response;

    bool res = client->GetLogFilterConfig(&request, &response, 0);
    for (auto i: response.srcids()) {
        srcids.push_back(i);
    }
    return srcids;
}


