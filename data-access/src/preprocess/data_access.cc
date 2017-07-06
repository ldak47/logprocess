#include "data_access.h"

namespace dataaccess {

std::string DataAccesserExcept[] = {
    "CLIENT_CREATE_FAIL",
    "PARTITION ILLEGAL"
};

DataAccesser::DataAccesser (std::string server_addr,
                            int rpc_timeout,
                            const libconfig::Setting &minos_cfg
                           ): status_(INITING), 
                              server_addr_(server_addr),
                              rpc_timeout_(rpc_timeout),
                              failure_str(""),
                              topic_id_(70029159),
                              token_("minos"),
                              default_package_id_(1), 
                              default_record_count_(1) {

    client_.reset(new RpcAdaptClient::PullDataRpcClient::PullClientHook(server_addr_, rpc_timeout_));
    if (!client_) {
        failure_str = DataAccesserExcept[0];
        LOG(WARNING) << "DataAccesser construct fail with reason " << failure_str;
    } else {
        Start(minos_cfg);
    }
}

DataAccesser::~DataAccesser () {
    client_.reset(0);
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

void DataAccesser::Subscribe (std::shared_ptr<boost::lockfree::queue<void *>> lockfreequeue) {
    baidu::minos::SubscribeRecordRequest request;
    baidu::minos::SubscribeRecordResponse response;
    request.set_topic_id(topic_id_);
    request.set_partition_group_id(partition_);
    request.mutable_account()->set_token(token_);
    request.set_data_package_id(default_package_id_);
    request.set_record_count(default_record_count_);
    status_ = RUNNING;
    
    while (1) {
        bool res = client_->Subscribe(&request, &response, 0);
        if (!res || response.status() != baidu::minos::SubscribeRecordResponse::SUCCEED) {
            if (response.status() != baidu::minos::SubscribeRecordResponse::SUCCEED &&
                response.status() != baidu::minos::SubscribeRecordResponse::NO_DATA_LEFT) {
                //not SUCCEED and not NO_DATA_LEFT, so that the broker pipelet happen except, need initsub again
                InitMinosPipe();
            } else {
                if (!res) {
                    //rpc request fail
                    LOG(WARNING) << "subscribe request fail";
                } else {
                    //other cases.....
                    LOG(WARNING) << "subscribe request get data fail, errormessage: " << response.error_message() << ", requestmsg: " << request.ShortDebugString() << ", errorcode: " << response.status() << ", cur_data_package_id: " << response.cur_data_package_id();
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

            if (body_len) {
                RawBody *newbody = new RawBody(body);
                lockfreequeue->push((void *)newbody);
                ++access_cnt_;
            } else {
                ++error_cnt_;
            }
            //LOG(INFO) << "tid: " << std::this_thread::get_id() << ", body: " << body;
        }
     
        //update ackage_id.
        default_package_id_ = response.cur_data_package_id() + 1;
        request.set_data_package_id(default_package_id_);
        
        //data in broker not so much, we could make a rest
        if (response.data_size() < default_record_count_) {
            std::chrono::seconds duration(1);
            std::this_thread::sleep_for(duration);
        }
        response.Clear();
    }
}

void DataAccesser::InitMinosPipe () {
    baidu::minos::InitSubscriberBrokerRequest init_request;
    baidu::minos::InitSubscriberBrokerResponse init_response;
    init_request.set_topic_id(topic_id_);
    init_request.set_partition_group_id(partition_);
    init_request.set_need_progress(false);
    init_request.mutable_account()->set_token(token_);
    init_request.set_type(baidu::minos::InitSubscriberBrokerRequest_Type_TIMESTAMP);
    init_request.set_start_timestamp(time(0));
    init_request.set_start_record_type(baidu::minos::InitSubscriberBrokerRequest::NEWEST_RECORD);

    bool res = client_->InitSub(&init_request, &init_response, 0);
    while (!res || init_response.status() != baidu::minos::InitSubscriberBrokerResponse::SUCCEED) {
        if (res && init_response.status() == baidu::minos::InitSubscriberBrokerResponse::STARTING) {
            std::chrono::seconds duration(10);
            std::this_thread::sleep_for(duration);
            return;
        }
     
        init_response.Clear();   
        std::chrono::seconds duration(1);
        std::this_thread::sleep_for(duration);
        res = client_->InitSub(&init_request, &init_response, 0);
    }

    LOG(INFO) << "partition " << partition_ << " initsub success";
}

bool DataAccesser::Start (const libconfig::Setting &minos_cfg) {
    if (!minos_arg_parse(minos_cfg)) {
        LOG(WARNING) << "DataAccesser Start fail with reason " << failure_str;
        return false;
    }

    InitMinosPipe();
}

};
