#include "gtest/gtest.h"
#include "glog/logging.h"
#include "range_iterator.h"
#include <mutex>
#include <chrono>
#include <thread>
#include <random>

class shareclass {
    std::mutex mtx;
public:
    shareclass(){}
    ~shareclass(){}

    void Put() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            LOG(INFO) << " PUT START ";
            int i = 0;
            while(i++ < 1000 * 1000 * 1000);
            LOG(INFO) << " PUT END ";
        }
    }

    void Get() {
        {
            std::unique_lock<std::mutex> lock(mtx);
            LOG(INFO) << " GET ";
        }
    }
};

class testone {
    shareclass &share;
public:
    testone(shareclass &_share):share(_share){}
    ~testone(){}
    void run () {
        share.Put();
    }
};

class testtwo {
    shareclass &share;
public:
    testtwo(shareclass &_share):share(_share){}
    ~testtwo(){}
    void run () {
        share.Get();
    }
};

TEST(test, mutex_in_class) {
    shareclass share;
    testone o(share);
    testtwo t(share);
    
    std::thread ta = std::thread([&o] () {
        while (1) {
            o.run();
            std::chrono::microseconds duration(10);
            std::this_thread::sleep_for(duration);
        }
    });
    std::thread tb = std::thread([&t] () {
        LOG(INFO) << "tb";
        while (1) {
            t.run();
        }
    });

    ta.join();
    tb.join();
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
