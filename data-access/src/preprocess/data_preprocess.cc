#include "data_preprocess.h"
#include "gflags/gflags.h"
#include "data_util.h"
#include "consistent_hash.h"

DEFINE_int32(WORKER_NUM, 12, "thread num for preprocess and rpc forward to rpc-process-server");
DEFINE_int32(DELAYTASK_WORKER_NUM, 1, "thread num for preprocess 'resno' msg, which judge whether should forward to rpc-process-server");
DEFINE_int32(LOCKFREEQUEUESIZE, 65535, "boost lockfreequeue size");
DEFINE_string(DATAPROCESSRPCSERVERADDR, "127.0.0.1", "data process rpc server addr");
DEFINE_int32(DATAPROCESSRPCSERVERPORT, 19999, "data process rpc server addr");
DEFINE_int32(DATAPROCESSRPCSERVERTIMEOUT, 300, "data process rpc server timeout");
DEFINE_int32(CONSISTENTHASH_VIRTUALNODENUM, 100, "consistent hash virtual node num per real server");

namespace dataaccess {

std::string failures[] = {
    "construct fail, threadpool construct fail",
    "construct fail, agent thread construct fail"
    "construct fail, redis construct or connect fail"
    "construct fail, delaytask threadpool construct fail",
};

extern std::shared_ptr<boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>> lockfreequeue_;
extern std::shared_ptr<boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>> lockfreequeue_new_;

DataPreProcess::DataPreProcess (const libconfig::Setting &cfg, 
                                DataFieldRuler &ruler, 
                                const std::string &rdsaddr, 
                                int rdsport,
                                const std::vector<std::string> &process_server_cfg):worker_num_(FLAGS_WORKER_NUM), failure_(""), stop_(true), delaytask_worker_num_(FLAGS_DELAYTASK_WORKER_NUM), ruler_(ruler) {
    loadbalancer_.reset(new common::ConsistentHash(process_server_cfg, process_server_cfg.size(), FLAGS_CONSISTENTHASH_VIRTUALNODENUM));
    cfg.lookupValue("worker_num", worker_num_);
    cfg.lookupValue("delaytask_worker_num", delaytask_worker_num_);
    if (worker_num_ <= 0 || delaytask_worker_num_ < 0) {
        worker_num_ = FLAGS_WORKER_NUM;
        delaytask_worker_num_ = FLAGS_DELAYTASK_WORKER_NUM;
    }
    redisctx_.reset(new common::RedisCli(rdsaddr, rdsport));
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
                LOG(ERROR) << "delaytask worker create fail.";
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
    //std::string srcid_regex = " srcid\\[(\\d+)\\] ";
    try {
        std::string field = "srcid[";
        size_t idx = body.find(field);
        if (idx != std::string::npos) {
            for (size_t i = idx + field.length(); i < body.length(); i++) {
                if (body[i] == ']') {
                    break;
                } else {
                    srcid += body[i];
                }
            }
        }
    //don't use any c/boost/c++11 regex in backend!
    #if 0
        std::regex reg(srcid_regex.c_str());
        std::smatch res;
        regex_search(body, res, reg);
        srcid = res.str(1);
    or
        const char *pattern = "srcid\\[([0-9]+)\\]";
        regex_t reg;
        regmatch_t pmatch[1];
        const size_t nmatch = 1;
        regcomp(&reg, pattern, REG_EXTENDED);
        int status = regexec(&reg, body.c_str(), nmatch, pmatch, 0);
        if (status == 0) {
            bool flag = false;
            for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
                if (!flag) {
                    if (body[i] == '[') {
                        flag = true;
                    }
                    continue;
                } else if (body[i] != ']') {
                    srcid += body[i];
                } else if (body[i] == ']') {
                    break;
                }            
            }
        }
        regfree(&reg);
    #endif
        
        return true;
    } catch (std::exception &err) {
        return false;
    }
}

std::map<std::string, std::string> DataPreProcess::GetFields (const std::string &body, const std::map<std::string, std::string> &field_regexes, bool type) {
    std::map<std::string, std::string> result;
    std::map<std::string, std::string>::const_iterator fit = field_regexes.begin();
    for (; fit != field_regexes.end(); fit++) {
        std::string field = fit->first, regex = fit->second, value = "";
	    try {
            if (field == "timestamp") {
                const char *pattern = regex.c_str();
                regex_t reg;
                regmatch_t pmatch[1];
                const size_t nmatch = 1;
                regcomp(&reg, pattern, REG_EXTENDED);
                int status = regexec(&reg, body.c_str(), nmatch, pmatch, 0);
                if (status == 0) {
                    value = "20";
                    for (size_t i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
                        value += body[i];
                    }
                    value[value.length() - 2] = '0';
                    value[value.length() - 1] = '0';
                }
                regfree(&reg);
            } else if (type == true && field == "resno") {
                std::string search = " errno[";
                size_t idx = body.find(search);
                if (idx != std::string::npos) {
                    idx = body.find(search, search.length());
                    if (idx != std::string::npos) {
                        for (size_t i = idx + search.length(); i < body.length(); i++) {
                            if (body[i] == ']') {
                                break;
                            } else {
                                value += body[i];
                            }
                        }

                        if (value == "0") {
                            value == "";
                        }
                    }
                }
            } else {
                std::string search = field + "[";
                if (field == "sid") {
                    search = " sid[";
                }
                size_t idx = body.find(search);
                if (idx != std::string::npos) {
                    for (size_t i = idx + search.length(); i < body.length(); i++) {
                        if (body[i] == ']') {
                            break;
                        } else {
                            value += body[i];
                        }
                    }
                }
            }

            result[field] = value;
	    } catch (...) {
	        continue;
	    }
    }
    
    return result;
}

void DataPreProcess::kv2request (const std::map<std::string, std::string> &kv, process::TransmitRequest &request, const std::map<std::string, bool> &istrans) {
    std::map<std::string, std::string>::const_iterator it = kv.begin();
    for (; it != kv.end(); it++) {
        std::string k = it->first, v = it->second;
        std::map<std::string, bool>::const_iterator iter = istrans.find(k);
        if (k != "type" && k != "application" && (iter == istrans.end() || iter->second == false)) {
            continue;
        }
        
        process::MapEntry *fields = request.add_values();
        fields->set_key(k);
        fields->set_value(v);
    }
}

void DataPreProcess::Transmit (const std::map<std::string, std::string> &kv, 
                               const std::map<std::string, bool> &istrans) {
    std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client_;
    std::string server_addr = GetProcessSerer(kv.find("query")->second);
    client_.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook(server_addr, FLAGS_DATAPROCESSRPCSERVERTIMEOUT));
        
    process::TransmitRequest request;
    process::TransmitResponse response;
    kv2request(kv, request, istrans);
    
    bool res = false;
    while (!(res = client_->Transmit(&request, &response, 0)));
#if 0
    if (!res) {
        LOG(WARNING) << "send to process-rpc-server failed, not repush into queue but async retry";
        retry_worker_->AddTask(std::function<void ()>([this, kv, istrans] {
            process::TransmitRequest request;
            process::TransmitResponse response;
            kv2request(kv, request, istrans);
            std::unique_ptr<RpcAdaptClient::PushDataRpcClient::PushClientHook> client_;
            std::string server_addr = GetProcessSerer(kv.find("query")->second);
            client_.reset(new RpcAdaptClient::PushDataRpcClient::PushClientHook(server_addr, FLAGS_DATAPROCESSRPCSERVERTIMEOUT));
            while (1) {
                bool res = client_->Transmit(&request, &response, 0);
                if (res) {
                    break;
                }
            }
            request.Clear();
        }));
    }
#endif 
    request.Clear();
    response.Clear();
/*
    std::string application = kv.find("application")->second, srcid = kv.find("srcid")->second, resno = kv.find("resno")->second;
    if (resno == "") {
        pvstater_.Inc(application, srcid, true, kv.find("timestamp")->second);
    } else {
        pvstater_.Inc(application, srcid, false, kv.find("timestamp")->second);
    }
*/
}

void DataPreProcess::PreProcess_new (void *msg) {
    std::string body = reinterpret_cast<struct RawBody *>(msg)->body_, srcid(""), cur_type("");
    delete (RawBody *)msg;
    if (!GetSrcid(body, srcid)) {
        return;
    }
    cur_type = ruler_.GetType(srcid);
    if (cur_type == "") {
        return;
    }
    std::map<std::string, std::string> field_regexes = ruler_.GetRule(cur_type);
    std::map<std::string, bool> field_istrans = ruler_.GetIsTrans(cur_type);
    std::map<std::string, std::string> kv = GetFields(body, field_regexes, true);
    kv["application"] = cur_type;
    kv["type"] = "new";
                 
    std::string query = kv["query"], qid = kv["qid"];
    std::string rawstr = query + "|" + srcid;
    uint32_t ha = common::FNV1_32_HASH(rawstr);
    std::string hashcode = std::to_string(ha);
    if (kv["resno"] != "") {
        if (redisctx_->Exists(qid) || redisctx_->Exists(hashcode)) {
    /*
            delaytask_workers_->AddDelayTask(1000 * 5, std::function<void ()>([this, hashcode, kv, field_istrans] {
                int err(0);
                std::string errmsg("");
                if (!redisctx_->Exists(hashcode) && !redisctx_->Exists(kv["qid"])) {
                    DataPreProcess::Transmit(kv, field_istrans);
                }
            }));
    */
            return;
        }
    } else {
        redisctx_->Set(hashcode, "OK", -1);
        redisctx_->Set(qid, "OK", 500);
    }

    Transmit(kv, field_istrans);
}

void DataPreProcess::PreProcess (void *msg) {
    //common::TimeCalcer tc;
    //double tm;
    std::string body = reinterpret_cast<struct RawBody *>(msg)->body_, srcid(""), cur_type("");
    delete (RawBody *)msg;
    if (!GetSrcid(body, srcid)) {
        return;
    }
    cur_type = ruler_.GetType(srcid);
    if (cur_type == "") {
        return;
    }
    std::map<std::string, std::string> field_regexes = ruler_.GetRule(cur_type);
    std::map<std::string, bool> field_istrans = ruler_.GetIsTrans(cur_type);
    std::map<std::string, std::string> kv = GetFields(body, field_regexes, false);
    kv["application"] = cur_type;
    kv["type"] = "old";
                 
    std::string query = kv["query"], qid = kv["qid"];
    std::string rawstr = query + "|" + srcid;
    uint32_t ha = common::FNV1_32_HASH(query);
    std::string hashcode = std::to_string(ha);
    if (kv["resno"] != "") {
        if (redisctx_->Exists(qid) || redisctx_->Exists(hashcode)) {
    /*
            delaytask_workers_->AddDelayTask(1000 * 15, std::function<void ()>([this, hashcode, kv, field_istrans] {
                int err(0);
                std::string errmsg("");
                if (!redisctx_->Exists(hashcode)) {
                    DataPreProcess::Transmit(kv, field_istrans);
                }
            }));
        }
    */
            return;
        }
    } else {
        redisctx_->Set(hashcode, "OK", -1);
        redisctx_->Set(qid, "OK", 500);
    }

    Transmit(kv, field_istrans);
//    tm = tc.Calc();
//    LOG(INFO) << std::fixed << "backcost " << tm;
}

bool DataPreProcess::Start () {
    stop_ = false;
    agent_.reset(new std::thread([this] () {
        while (1) {
            std::unique_lock<std::mutex> lock(mtx_);
            if (!lockfreequeue_.get() || (lockfreequeue_->empty() && !stop_)) {
                std::cv_status cvsts = cond_.wait_for(lock, std::chrono::milliseconds(100));
                if (cvsts == std::cv_status::timeout) {
                    continue;
                }
            }
            if (stop_) {
                break;
            }
            
            void *msg = nullptr;
            lockfreequeue_->pop(msg);
            if (msg) {
                Task task = std::bind(&DataPreProcess::PreProcess, this, msg);
                workers_->AddTask(task);
            }
        }

        LOG(INFO) << "agent thread exit.";
    }));
    agent_new_.reset(new std::thread([this] () {
        while (1) {
            if (!lockfreequeue_.get() || (lockfreequeue_->empty() && !stop_)) {
                std::chrono::milliseconds duration(100);
                std::this_thread::sleep_for(duration);
                continue;
            }
            if (stop_) {
                break;
            }
            
            void *msg = nullptr;
            lockfreequeue_new_->pop(msg);
            if (msg) {
                Task task = std::bind(&DataPreProcess::PreProcess, this, msg);
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
}











};
