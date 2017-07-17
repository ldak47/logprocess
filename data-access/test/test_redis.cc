#include "gtest/gtest.h"
#include "glog/logging.h"
#include <string>
#include <hiredis/hiredis.h>

TEST(test, test1) {
    std::string ip = "127.0.0.1";
    int port = 6379;
    redisContext* c = redisConnect(ip.c_str(), 6379);
    if (c->err) {
        redisFree(c);
        LOG(ERROR) << "redis connect " << ip << ":" << port << " fail";
        return;
    }

    const char* command1 = "set stest1 value1"; 
    redisReply *r = (redisReply *)redisCommand(c, command1);
    if(!r) {
        redisFree(c);
        LOG(ERROR) << "redis set fail";
        return;
    }
    freeReplyObject(r);
    LOG(INFO) << "Succeed to execute command " << command1;

    const char* command0 = "exists stest1";
    r = (redisReply *)redisCommand(c, command0);
    if (!r) {
        redisFree(c);
        LOG(ERROR) << "redis exists fail";
        return;
    }
    LOG(INFO) << r->type;
    LOG(INFO) << r->integer;
    freeReplyObject(r);
    
    const char* command2 = "expire stest1 2000";
    r = (redisReply *)redisCommand(c, command2);
    if(!r) {
        redisFree(c);
        LOG(ERROR) << "redis expire fail";
        return;
    }
    freeReplyObject(r);
    LOG(INFO) << "Succeed to execute command " << command2;

    const char* command3 = "get stest1";
    r = (redisReply *)redisCommand(c, command3);
    if(!r) {
        redisFree(c);
        LOG(ERROR) << "redis get fail";
        return;
    }
    LOG(INFO) << "Succeed to execute command " << command3;
    LOG(INFO) << command3 << ": " << r->str << "|" << r->type;
    freeReplyObject(r);

    const char* command4 = "get fsafsasdfsa";
    r = (redisReply *)redisCommand(c, command4);
    if(!r) {
        redisFree(c);
        LOG(ERROR) << "redis get fail";
        return;
    }
    LOG(INFO) << "Succeed to execute command " << command4;
    if (r->str) {
        LOG(INFO) << command4 << ": " << r->str << "|" << r->type;
    } else {
        LOG(INFO) << command4 << " r->type: " << r->type;
    }
    freeReplyObject(r);

}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
