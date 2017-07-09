#include "gtest/gtest.h"
#include "glog/logging.h"
#include "singleton.h"
#include <range_iterator.h>
#include <thread>
#include <chrono>

struct test1 {
    int a_;
    test1():a_(1) {
        LOG(INFO) << std::this_thread::get_id();
    }
    void set(int a){
        a_ = a;
    }
    int get(){
        return a_;
    }
};

struct test2 {
    int b_;
    test2():b_(100) {
        LOG(INFO) << std::this_thread::get_id();
    }
    void set(int b){
        b_ = b;
    }
    int get(){
        return b_;
    }
};

TEST(test_singleton, test1) {
    std::vector<std::thread> ths(10);
    for (int i: common::Range(0, 10)) {
        ths[i] = std::thread([i] () {
            int c = 10;
            while (c) {
                test1 *t = common::Singleton<test1>::get();
                t->set(i);
                LOG(INFO) << std::this_thread::get_id() << ": " << t << ": " << t->get();
                test2 *s = common::Singleton<test2>::get();
                s->set(i);
                LOG(INFO) << std::this_thread::get_id() << ": " << s << ": " << s->get();
                --c;
                std::chrono::seconds duration(1);
                std::this_thread::sleep_for(duration);
            }
        });
    }

    for (int i: common::Range(0, 10)) {
        ths[i].join();
    }
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
