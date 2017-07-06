#include "client_hook.h"
#include "range_iterator.h"
#include "glog/logging.h"
#include "gflags/gflags.h"
#include "libconfig.h++"
#include "thread_pool.h"
#include <string>
#include <chrono>
#include <thread>

namespace dataprocess {

    class Process_Impl {
    public:
        Process_Impl(){}
        ~Process_Impl(){}
        void DataReceive(const transmit::TransmitRequest *request,
                         transmit::TransmitResponse *response,
                         ::google::protobuf::Closure *done);
    };

    class Manager_Impl {
    public:
        Manager_Impl(){}
        ~Manager_Impl(){}
    };
};
