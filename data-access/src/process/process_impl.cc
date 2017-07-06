#include "process_impl.h"

namespace dataprocess {

void Process_Impl::DataReceive (const transmit::TransmitRequest *request,
                                transmit::TransmitResponse *response,
                                ::google::protobuf::Closure *done) {
    int fieldnum = request->values_size();
    for (int i: common::Range(0, fieldnum)) {
        transmit::MapEntry kv = request->values(i);
        std::string key = kv.key(), value = kv.value();
        LOG(INFO) << key << ": " << value;
    }

    //here actual process...
    
    response->set_status(transmit::TransmitResponse_Res_Status_SUCCEED);
    done->Run();
}

};
