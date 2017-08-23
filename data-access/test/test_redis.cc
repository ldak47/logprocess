#include "gtest/gtest.h"
#include "glog/logging.h"
#include <string>
#include <hiredis/hiredis.h>
#include "libconfig.h++"
#include "redis.h"

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

TEST(test, zset) {
    libconfig::Config cfg;
    cfg.readFile("/home/work/data-access/conf/data-access.cfg");
    const libconfig::Setting &root = cfg.getRoot();
    common::RedisCli rds(root["redis_cfg"]);

    std::string setname = "BB7E8EDD12B162939FE08F82E425823F";
    rds.Zset(setname, "南京天气", 1500297900);
    return;
    
    rds.Zset(setname, "v0", 110);
    rds.Zset(setname, "v1", 111);
    rds.Zset(setname, "v2", 112);
    rds.Zset(setname, "v3", 113);
    rds.Zset(setname, "v4", 114);
    rds.Zset(setname, "v5", 115);
    rds.Zset(setname, "v6", 117);
    rds.Zset(setname, "v7", 119);
    rds.Zset(setname, "v8", 120);
    rds.Zset(setname, "v9", 121);
    rds.Zset(setname, "v10", 150);
    
    uint64_t cnt = 0;
    rds.Zcount(setname, cnt, 110, 121);
    LOG(INFO) << cnt;
    rds.Zcount(setname, cnt, 110, 150);
    LOG(INFO) << cnt;
    rds.Zcount(setname, cnt, 0, -1);
    LOG(INFO) << cnt;

    rds.ZRem(setname, 111, 113);    //rm 111 112 113
    rds.Zcount(setname, cnt, 0, -1);
    LOG(INFO) << cnt;
    rds.ZRem(setname, 0, 1, false); //rm 110 114
    rds.Zcount(setname, cnt, 0, -1);
    LOG(INFO) << cnt;

    std::vector<std::string> values;
    rds.Zrange(setname, values, 0, 1);  //should 115 116
    for (auto v: values) {
        LOG(INFO) << v;
    }
    values.clear();
    rds.Zrange(setname, values, 0, 1, false);  //should 150 121
    for (auto v: values) {
        LOG(INFO) << v;
    }
    values.clear();
    rds.ZrangeByScore(setname, values, std::to_string(117), std::to_string(120), false);  //should 120 119 117
    for (auto v: values) {
        LOG(INFO) << v;
    }
    values.clear();
    rds.ZrangeByScore(setname, values, "", "", false);  //should 150 - 115
    for (auto v: values) {
        LOG(INFO) << v;
    }
    values.clear();
    rds.ZrangeByScore(setname, values, "", "", true);  //should 115 - 150
    for (auto v: values) {
        LOG(INFO) << v;
    }
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
