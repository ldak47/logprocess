#include "data_preprocess.h"
#include "gflags/gflags.h"
#include "boost/lockfree/queue.hpp"
#include "data_util.h"

DEFINE_int32(WORKER_NUM, 40, "thread num for preprocess and rpc forward to rpc-process-server");
DEFINE_int32(LOCKFREEQUEUESIZE, 10000, "boost lockfreequeue size");
DEFINE_string(DATAPROCESSRPCSERVERADDR, "127.0.0.1:20000", "data process rpc server addr");
DEFINE_int32(DATAPROCESSRPCSERVERTIMEOUT, 30, "data process rpc server timeout");

namespace dataaccess {

std::string failures[] = {
    "construct fail, threadpool construct fail",
    "construct fail, agent thread construct fail"
};

extern std::shared_ptr<boost::lockfree::queue<void *>> lockfreequeue_;

DataPreProcess::DataPreProcess (const libconfig::Setting &cfg, DataFieldRuler &ruler):worker_num_(FLAGS_WORKER_NUM), 
                                                                                      failure_(""), 
                                                                                      stop_(true) {
    lockfreequeue_.reset(new boost::lockfree::queue<void *>(FLAGS_LOCKFREEQUEUESIZE));
    ruler_ = std::move(ruler);
    cfg.lookupValue("worker_num", worker_num_);
    if (worker_num_ <= 0) {
        worker_num_ = FLAGS_WORKER_NUM;
    }
    workers_.reset(new ThreadPool(worker_num_));
    if (!workers_.get()) {
        LOG(ERROR) << "workers create fail.";
        failure_ = failures[0];
    } else {
        Start();
    }
}

DataPreProcess::~DataPreProcess () {
    Stop(false);
}

bool DataPreProcess::Start () {
    stop_ = false;
    agent_.reset(new std::thread([this] () {
        while (1) {
            std::unique_lock<std::mutex> lock(mtx_);
            if (lockfreequeue_->empty() && !stop_) {
                std::cv_status cvsts = cond_.wait_for(lock, std::chrono::milliseconds(100));
                if (cvsts == std::cv_status::timeout) {
                    continue;
                }
            }
            if (stop_) {
                break;
            }
            
            void *msg((void *)nullptr);
            lockfreequeue_->pop(msg);
            if (msg) {
                Task task(std::function<void ()> ([this, msg] {
                    std::string body = reinterpret_cast<struct RawBody *>(msg)->body_, cur_type = "weather";

                    std::map<std::string, std::string> field_regexes = ruler_.GetRule(cur_type);
                    std::map<std::string, std::string>::iterator fit = field_regexes.begin();
                    
                    bool is_weather(false);
                    transmit::TransmitRequest request;
                    transmit::TransmitResponse response;
                    std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client_;

                    //shoule be a "load balance" module here. now just 127.0.0.1:20000
                    std::string server_addr = FLAGS_DATAPROCESSRPCSERVERADDR;
                    
                    client_.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook(server_addr, FLAGS_DATAPROCESSRPCSERVERTIMEOUT));
                    for (; fit != field_regexes.end(); fit++) {
                        std::string field = fit->first, regex = fit->second, value;
                        try {
                            std::regex reg(regex.c_str());
                            std::smatch res;
                            regex_search(body, res, reg);
                            value = res.str(1);
                            //LOG(INFO) << field << ": " << value;

                            transmit::MapEntry *fields = request.add_values();
                            fields->set_key(field);
                            fields->set_value(value);
                            if (field == "srcid" && (res.str(1) == "4178" || res.str(1) == "4186" || res.str(1) == "4166") ) {
                                is_weather = true;
                            }
                        } catch (...) {
                            continue;
                        }
                    }
                    if (is_weather) {
                        bool res = client_->Transmit(&request, &response, 0);
                        if (!res) {
                            LOG(ERROR) << "send to process-rpc-server failed...";
                        }
                    }
                    delete (RawBody *)msg;
                }));
                workers_->AddTask(task);
            }
        }

        LOG(INFO) << "agent thread exit.";
    }));
    if (!agent_.get()) {
        failure_ = failures[1];
    } 
}

bool DataPreProcess::Stop (bool graceful) {
    std::unique_lock<std::mutex> lock(mtx_);
    if (graceful) {
        while (!lockfreequeue_->empty()) {
            std::chrono::seconds duration(1);
            std::this_thread::sleep_for(duration);
        }
    }
    stop_ = true;
    cond_.notify_one();
    std::chrono::seconds duration(10);
    std::this_thread::sleep_for(duration);
    workers_->Stop(graceful);

    if (lockfreequeue_.get()) {
        boost::lockfree::queue<void *> *t = lockfreequeue_.get();
        delete t;
        t = 0;
        lockfreequeue_.reset(t);
    }

    if (workers_.get()) {
        delete workers_.get();
        workers_.reset(nullptr);
    }
}











};
