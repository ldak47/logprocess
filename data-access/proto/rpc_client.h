#ifndef DATAACCESS_PROTO_RPCCLIENT_H
#define DATAACCESS_PROTO_RPCCLIENT_H
#include <sofa/pbrpc/pbrpc.h>
#include <string>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <functional>
#include <glog/logging.h>

namespace RpcAdaptClient {

template<class REQ, class RESP, class CB> struct RPC_CB_PARAMS {
    sofa::pbrpc::RpcController *rpc_controller_;
    const REQ *request_;
    RESP *response_;
    CB callback_;
    std::string notice_;
    RPC_CB_PARAMS(sofa::pbrpc::RpcController *cntl,
                  const REQ *request,
                  RESP *response,
                  const std::string &notice,
                  CB callback): rpc_controller_(cntl),
                                request_(request),
                                response_(response),
                                notice_(notice),
                                callback_(callback) {}
};

};
#endif
