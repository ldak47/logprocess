#include "data_preprocess.h"
#include "gflags/gflags.h"
#include "data_util.h"

DEFINE_int32(WORKER_NUM, 40, "thread num for preprocess and rpc forward to rpc-process-server");
DEFINE_int32(DELAYTASK_WORKER_NUM, 40, "thread num for preprocess 'resno' msg, which judge whether should forward to rpc-process-server");
DEFINE_int32(LOCKFREEQUEUESIZE, 10000, "boost lockfreequeue size");
DEFINE_string(DATAPROCESSRPCSERVERADDR, "127.0.0.1", "data process rpc server addr");
DEFINE_int32(DATAPROCESSRPCSERVERPORT, 20000, "data process rpc server addr");
DEFINE_int32(DATAPROCESSRPCSERVERTIMEOUT, 30, "data process rpc server timeout");
DEFINE_int32(CONSISTENTHASH_VIRTUALNODENUM, 100, "consistent hash virtual node num per real server");

namespace dataaccess {

std::string failures[] = {
    "construct fail, threadpool construct fail",
    "construct fail, agent thread construct fail"
    "construct fail, redis construct or connect fail"
    "construct fail, delaytask threadpool construct fail",
};

extern std::shared_ptr<boost::lockfree::queue<void *>> lockfreequeue_;

DataPreProcess::DataPreProcess (const libconfig::Setting &cfg, DataFieldRuler &ruler, const libconfig::Setting &rdscfg, const std::vector<std::string> &process_server_cfg):worker_num_(FLAGS_WORKER_NUM), failure_(""), stop_(true), delaytask_worker_num_(FLAGS_DELAYTASK_WORKER_NUM) {
    loadbalancer_.reset(new common::ConsistentHash(process_server_cfg, process_server_cfg.size(), FLAGS_CONSISTENTHASH_VIRTUALNODENUM));
    lockfreequeue_.reset(new boost::lockfree::queue<void *>(FLAGS_LOCKFREEQUEUESIZE));
    ruler_ = std::move(ruler);
    cfg.lookupValue("worker_num", worker_num_);
    cfg.lookupValue("delaytask_worker_num", delaytask_worker_num_);
    if (worker_num_ <= 0) {
        worker_num_ = FLAGS_WORKER_NUM;
    }
    redisctx_.reset(new common::RedisCli(rdscfg));
    std::string err;
    if (!redisctx_.get() || redisctx_->GetError(err)) {
        LOG(ERROR) << "workers create fail.";
        failure_ = failures[2];
    } else {
        workers_.reset(new ThreadPool(worker_num_));
        if (!workers_.get()) {
            LOG(ERROR) << "workers create fail.";
            failure_ = failures[0];
        } else {
            delaytask_workers_.reset(new ThreadPool(delaytask_worker_num_));
            if (!delaytask_workers_.get()) {
                LOG(ERROR) << "workers create fail.";
                failure_ = failures[3];        
            } else {
                Start();
            }
        }
    }
}

DataPreProcess::~DataPreProcess () {
    Stop(false);
}

std::string DataPreProcess::GetProcessSerer (std::string query) {
    return loadbalancer_->GetServerNode(query);
}

bool DataPreProcess::GetSrcid (const std::string &body, std::string &srcid) {
    std::string srcid_regex = " srcid\\[(\\d+)\\] ";
    try {
        std::regex reg(srcid_regex.c_str());
        std::smatch res;
        regex_search(body, res, reg);
        srcid = res.str(1);
        return true;
    } catch (std::exception &err) {
        return false;
    }
}

std::map<std::string, std::string> DataPreProcess::GetFields (const std::string &body, const std::map<std::string, std::string> &field_regexes) {
    std::map<std::string, std::string> result;
    std::map<std::string, std::string>::const_iterator fit = field_regexes.begin();
    for (; fit != field_regexes.end(); fit++) {
        std::string field = fit->first, regex = fit->second, value;
	    try {
	        std::regex reg(regex.c_str());
	        std::smatch res;
	        regex_search(body, res, reg);
	        value = res.str(1);
	        if (field == "timestamp") {
	            value[value.length() - 2] = value[value.length() - 1] = '0';
	        }
	
            result[field] = value;
	    } catch (...) {
	        continue;
	    }
    }
    
    return result;
}

void DataPreProcess::kv2request (const std::map<std::string, std::string> &kv, process::TransmitRequest &request) {
    std::map<std::string, std::string>::const_iterator it = kv.begin();
    for (; it != kv.end(); it++) {
        std::string k = it->first, v = it->second;
        process::MapEntry *fields = request.add_values();
        fields->set_key(k);
        fields->set_value(v);
    }
}

void DataPreProcess::Transmit (const std::map<std::string, std::string> &kv, const std::shared_ptr<boost::lockfree::queue<void *>> &lockfreequeue_, void *msg) {
    std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client_;
    std::string server_addr = GetProcessSerer(kv.find("query")->second);
    client_.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook(server_addr, FLAGS_DATAPROCESSRPCSERVERTIMEOUT));
        
    process::TransmitRequest request;
    process::TransmitResponse response;
    kv2request(kv, request);
    
    bool res = client_->Transmit(&request, &response, 0);
    if (!res) {
        LOG(ERROR) << "send to process-rpc-server failed, repush into queue...";
        lockfreequeue_->push(msg);
    } else {
        delete (RawBody *)msg;
    }
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
                    static int remote_idx = FLAGS_DATAPROCESSRPCSERVERPORT;
                    std::string body = reinterpret_cast<struct RawBody *>(msg)->body_, srcid(""), cur_type("");
                    if (!GetSrcid(body, srcid)) {
                        delete (RawBody *)msg;
                        return;
                    }
                    cur_type = ruler_.GetType(srcid);
                    if (cur_type == "") {
                        delete (RawBody *)msg;
                        return;
                    }
                    std::map<std::string, std::string> field_regexes = ruler_.GetRule(cur_type);
                    std::map<std::string, std::string> kv = GetFields(body, field_regexes);
                    kv["application"] = cur_type;
                    
                    std::string qid = kv["qid"];
                    int err(0);
                    std::string errmsg("");
                    if (kv["resno"] != "") {
                        if (redisctx_->Exists(qid, err, errmsg)) {
                            delete (RawBody *)msg;
                            return;
                        } else {
                            delaytask_workers_->AddDelayTask(1000 * 10000, std::function<void ()>([this, msg, qid, lockfreequeue_, kv] {
                                int err(0);
                                std::string errmsg("");
                                if (redisctx_->Exists(qid, err, errmsg)) {
                                    delete (RawBody *)msg;
                                } else {
                                    DataPreProcess::Transmit(kv, lockfreequeue_, msg);
                                }
                            }));
                        }

                        return;
                    } else {
                        redisctx_->Set(qid, kv["query"], err, errmsg);
                    }

                    Transmit(kv, lockfreequeue_, msg);
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
