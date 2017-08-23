#include "data_access.h"

extern Health health;

namespace dataaccess {

extern std::shared_ptr<boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>> lockfreequeue_;
extern std::shared_ptr<boost::lockfree::queue<void *, boost::lockfree::fixed_sized<true>>> lockfreequeue_new_;

//application(string), srcid(string), resno(bool), timestamp(string)
using pvstat_content = std::tuple<std::string, std::string, bool, std::string>;

std::string DataAccesserExcept[] = {
    "CLIENT_CREATE_FAIL",
    "PARTITION ILLEGAL"
};

DataAccesser::DataAccesser (std::string server_addr,
                            int rpc_timeout,
                            const libconfig::Setting &minos_cfg,
                            const std::vector<std::string> &support_type,
                            const libconfig::Setting &srcids_cfg,
                            AccessFilter &datafilter,
                            int topic_id,
                            std::string token,
                            bool type,
                            bool &pullswitch,
                            PvStater &pvstater,
                            DataFieldRuler &ruler,
                            std::string &platformaddr
                           ): status_(INITING), 
                              server_addr_(server_addr),
                              rpc_timeout_(rpc_timeout),
                              failure_str(""),
                              topic_id_(topic_id),
                              token_(token),
                              default_package_id_(1), 
                              default_record_count_(1),
                              datafilter_(datafilter),
                              type_(type),
                              pullswitch_(pullswitch),
                              pvstater_(pvstater),
                              ruler_(ruler),
                              platformaddr_(platformaddr)
                              {

    client_.reset(new RpcAdaptClient::PullDataRpcClient::PullClientHook(server_addr_, rpc_timeout_));
    if (!client_) {
        failure_str = DataAccesserExcept[0];
        LOG(ERROR) << "DataAccesser construct client_ fail with reason " << failure_str;
    } else {
        resender.reset(new ThreadPool(8));
        if (!resender.get()) {
            failure_str = DataAccesserExcept[0];
            LOG(ERROR) << "DataAccesser construct resender fail with reason " << failure_str;
        } else {
            Start(minos_cfg, support_type, srcids_cfg);
        }
    }
}

DataAccesser::~DataAccesser () {
    client_.reset(0);
}

bool DataAccesser::filter (const std::string &msg) {
    if (msg.find("WARNING") != std::string::npos) {
        return false;
    }

    try {
        std::string field = "srcid[";
        size_t idx = msg.find(field);
        std::string srcid = "";
        if (idx != std::string::npos) {
            for (size_t i = idx + field.length(); i < msg.length(); i++) {
                if (msg[i] == ']') {
                    break;
                } else {
                    srcid += msg[i];
                }
            }
        }
        
    //don't use any c/boost/c++11 regex in backend!
    #if 0
        std::string srcid_regex = " srcid\\[(\\d+)\\] ";
        std::regex reg(srcid_regex.c_str());
        std::smatch res;
        regex_search(msg, res, reg);
        std::string srcid = res.str(1);
    or
        std::string srcid = "";
        const char *pattern = "srcid\\[([0-9]+)\\]";
        regex_t reg;
        regmatch_t pmatch[1];
        const size_t nmatch = 1;
        regcomp(&reg, pattern, REG_EXTENDED);
        int status = regexec(&reg, msg.c_str(), nmatch, pmatch, 0);
        if (status == 0) {
            bool flag = false;
            for (int i = pmatch[0].rm_so; i < pmatch[0].rm_eo; i++) {
                if (!flag) {
                    if (msg[i] == '[') {
                        flag = true;
                    }
                    continue;
                } else if (msg[i] != ']') {
                    srcid += msg[i];
                } else if (msg[i] == ']') {
                    break;
                }            
            }
        } else {
            return false;
        }
        regfree(&reg);
    #endif

        if (!datafilter_.Judge(srcid)) {
            return false;
        }
    } catch (std::exception &err) {
        return false;
    }

    return true;
}

bool DataAccesser::minos_arg_parse (const libconfig::Setting &minos_cfg) {
    int topic_id, partition, default_package_id, default_record_count;
    std::string token;
    minos_cfg.lookupValue("topic_id", topic_id);
    minos_cfg.lookupValue("token", token);
    minos_cfg.lookupValue("package_id", default_package_id);
    minos_cfg.lookupValue("record_count", default_record_count);
    minos_cfg.lookupValue("partition", partition);
    if (topic_id > 0) {
        topic_id_ = topic_id;
    }
    if (default_package_id > 0) {
        default_package_id_ = default_package_id;
    }
    if (default_record_count > 0) {
        default_record_count_ = default_record_count;
    }
    if (token != "") {
        token_ = token;
    }
    if (partition <= 0) {
        failure_str = DataAccesserExcept[1];
        return false;
    } else {
        partition_ = partition;
        return true;
    }
}

std::string DataAccesser::Get_Timestamp (const std::string &body) {
    std::string value("");
    std::string regex = "([0-9]+\-[0-9]+\-[0-9]+ [0-9]+\:[0-9]+\:[0-9]+)";
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
    return value;
}

std::string DataAccesser::Get_Srcid (const std::string &body) {
    std::string value("");
    std::string search = "srcid[";
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

    return value;
}

bool DataAccesser::Get_Resno (const std::string &body) {
    std::string value("");
    if (type_) {
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
        std::string search = "resno[";
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

    return (value == "");
}

std::string DataAccesser::Get_Application (const std::string &srcid) {
    std::string cur_type = ruler_.GetType(srcid);
    return cur_type;
}

bool DataAccesser::GetRelaxingNode (std::string &freenode) {
    common::HttpClient cli;
    int rescode = 0, try_times = 3;
    std::string plathttpaddr_relax = platformaddr_ + "/relax";
    std::string request(""), response("");
    while ((rescode = cli.Get(plathttpaddr_relax, response)) && try_times) {
        --try_times;
    }
    if (rescode) {
        LOG(WARNING) << "find relaxing nodes fail! with libcurl code " << rescode;
        return false;
    } else {
        cJSON *root = cJSON_Parse(response.c_str());
        if (!root) {
            LOG(WARNING) << "parse json fail with response " << response;
            return false;
        }
        cJSON *success = cJSON_GetObjectItem(root, "success");
        cJSON *message = cJSON_GetObjectItem(root, "message");
        if (!success) {
            LOG(WARNING) << "parse json field success fail with response " << response;
            cJSON_Delete(root);
            return false;
        }
        if (success->valueint != 0) {
            LOG(WARNING) << "parse json field success result is not 0";
            cJSON_Delete(root);
            return false;
        }
        freenode = message->valuestring;
        //LOG(INFO) << "because of health(vmres >= 1GB), and find relaxing nodes " << freenode << " then will retransmit";
        cJSON_Delete(root);
        return true;
    }
}

void DataAccesser::Subscribe () {
    baidu::minos::SubscribeRecordRequest request;
    baidu::minos::SubscribeRecordResponse response;
    request.set_topic_id(topic_id_);
    request.set_partition_group_id(partition_);
    request.mutable_account()->set_token(token_);
    request.set_data_package_id(default_package_id_);
    request.set_record_count(default_record_count_);
    status_ = RUNNING;
    
    while (1) {
        if (!pullswitch_) {
            std::chrono::seconds duration(1);
            std::this_thread::sleep_for(duration);
        }
        bool res = client_->Subscribe(&request, &response, 0);
        if (!res || response.status() != baidu::minos::SubscribeRecordResponse::SUCCEED) {
            if (response.status() != baidu::minos::SubscribeRecordResponse::SUCCEED &&
                response.status() != baidu::minos::SubscribeRecordResponse::NO_DATA_LEFT) {
                //not SUCCEED and not NO_DATA_LEFT, so that the broker pipelet happen except, need initsub again
                //LOG(WARNING) << "not SUCCEED and not NO_DATA_LEFT(status code: " << response.status() << "), so that the broker pipelet happen except, need initsub again";
                InitMinosPipe();
            } else {
                if (!res) {
                    //rpc request fail
                    LOG(WARNING) << "type " << type_ << " subscribe request fail";
                } else {
                    //other cases(such as 'no data left.'), have a rest.....
                    if (response.error_message() == "no data left.") {
                        std::chrono::seconds duration(2);
                        std::this_thread::sleep_for(duration);
                        LOG(INFO) << "type " << type_ << " no data left.";
                    } else {
                        LOG(WARNING) << "type " << type_ << " subscribe request get data fail, errormessage: " << response.error_message() << ", requestmsg: " << request.ShortDebugString() << ", errorcode: " << response.status() << ", cur_data_package_id: " << response.cur_data_package_id();
                    }
                }
            }

            continue;
        }

        //parse the content-body from message.
        for (auto i: common::Range(0, response.data_size())) {
            const baidu::minos::PbRecord record = response.data(i);
            const baidu::minos::PbRecordHeader head = record.header();
            int pipelet_id = head.partition_id();
            uint64_t seq_id = head.record_id();
            int body_len = head.body_length();
            std::string body = "";
            for (int i = 0; i < body_len; i++) {
                if (record.body()[i] == '\n' || record.body()[i] == '\0') {
                    body += ' ';
                } else {
                    body += record.body()[i];
                }
            }

            if (body_len && filter(body)) {
                ++access_cnt_;
                //pv stat
                std::string srcid = Get_Srcid(body);
                std::string pvstat_type = Get_Application(srcid);
                if (pvstat_type != "") {
                    std::string timestamp = Get_Timestamp(body);
                    bool resno = Get_Resno(body);
                    pvstater_.Inc(pvstat_type, srcid, resno, timestamp);
                }
                
                //judge whether push-to-lfq or resend-to-relaxingnode
                uint32_t mem = std::get<0>(health);
                if (mem >= 1000 * 1024) {
                    auto task = std::function<void ()>([this, body] {
                        std::string relaxing_node("");
                        if (GetRelaxingNode(relaxing_node)) {
                            std::unique_ptr<RpcAdaptClient::ManageRpcClient::ManageClientHook> client;
                            client.reset(new RpcAdaptClient::ManageRpcClient::ManageClientHook(relaxing_node, 3000)); 
                            manage::RetransmitRequest request;
                            manage::RetransmitResponse response;
                            request.set_type(type_);
                            request.set_package(body);
                            while(true != client->Retransmit(&request, &response, 0));
                        }
                    });
                    resender->AddTask(task);
                    std::chrono::microseconds duration(1000 * 100);
                    std::this_thread::sleep_for(duration);    
                    continue;
                } else {
                    RawBody *newbody = new RawBody(body);
                    bool status = false;

                    if (type_) {
                        while (!(status = lockfreequeue_new_->push((void *)newbody))) {
                            LOG(WARNING) << "waiting process several...";
                            std::chrono::microseconds duration(1000 * 1000 * 10);
                            std::this_thread::sleep_for(duration);    
                        }
                    } else {
                        while (!(status = lockfreequeue_->push((void *)newbody))) {
                            LOG(WARNING) << "waiting process several...";
                            std::chrono::microseconds duration(1000 * 1000 * 10);
                            std::this_thread::sleep_for(duration);    
                        }
                    }
                }
            }
        }
     
        //update ackage_id.
        default_package_id_ = response.cur_data_package_id() + 1;
        request.set_data_package_id(default_package_id_);
        
        //data in broker not so much, we could make a rest
        response.Clear();
        if (response.data_size() < default_record_count_) {
            std::chrono::seconds duration(1);
            std::this_thread::sleep_for(duration);
        }
    }
}

void DataAccesser::InitMinosPipe () {
    baidu::minos::InitSubscriberBrokerRequest init_request;
    baidu::minos::InitSubscriberBrokerResponse init_response;
    init_request.set_topic_id(topic_id_);
    init_request.set_partition_group_id(partition_);
    init_request.set_need_progress(false);
    init_request.mutable_account()->set_token(token_);
    init_request.set_type(baidu::minos::InitSubscriberBrokerRequest_Type_RECORD_ID);
    init_request.set_start_timestamp(time(0));
    init_request.set_start_record_type(baidu::minos::InitSubscriberBrokerRequest::NEWEST_RECORD);

    LOG(INFO) << "type " << type_ << " begin init minos mq broker...";
    bool res = client_->InitSub(&init_request, &init_response, 0);
    while (!res || init_response.status() != baidu::minos::InitSubscriberBrokerResponse::SUCCEED) {
        LOG(INFO) << "type " << type_ << " init-status is : " << init_response.status();
        if (res && init_response.status() == baidu::minos::InitSubscriberBrokerResponse::STARTING) {
            std::chrono::seconds duration(10);
            std::this_thread::sleep_for(duration);
        }
     
        init_response.Clear();   
        std::chrono::seconds duration(1);
        std::this_thread::sleep_for(duration);
        res = client_->InitSub(&init_request, &init_response, 0);
    }

    LOG(INFO) << "type " << type_ << ", partition " << partition_ << " initsub success";
}

bool DataAccesser::Start (const libconfig::Setting &minos_cfg, 
                          const std::vector<std::string> &support_type,
                          const libconfig::Setting &srcids_cfg) {
    if (!minos_arg_parse(minos_cfg)) {
        LOG(WARNING) << "DataAccesser Start fail with reason " << failure_str;
        return false;
    }
    InitMinosPipe();
}

};
