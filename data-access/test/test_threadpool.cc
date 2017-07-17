#include "gtest/gtest.h"
#include "glog/logging.h"
#include "thread_pool.h"

void divisiontask (int a, int b) {
    if (!b) {
        LOG(INFO) << "division b must be non 0!";
        return;
    }
    LOG(INFO) << a << " / " << b << " = " << a / b;
}

void multipletask (int a, int b) {
    LOG(INFO) << a << " * " << b << " = " << a * b;
}

void minostask (int a, int b) {
    LOG(INFO) << a << " - " << b << " = " << a - b;
}

void addtask (int a, int b) {
    LOG(INFO) << a << " + " << b << " = " << a + b;
}

bool test (ThreadPool &thread_pool) {
    Task taskadd(std::bind(&addtask, 1, 2));
    Task taskminos(std::bind(&minostask, 1, 2));
    Task taskmultiple(std::bind(&multipletask, 1, 2));
    thread_pool.AddTimerTask(1000 * 2000, taskadd);
    thread_pool.AddDelayTask(1000 * 5000, taskminos);
    for (int i = 0; i < 1000; i++) {
        thread_pool.AddTask(taskmultiple);
        auto task = std::bind(divisiontask, 1, 2);
        thread_pool.AddTask(task);
    }
    auto task = std::bind(&divisiontask, std::placeholders::_1, 2);
    task(100000);

    std::chrono::seconds duration(10);
    std::this_thread::sleep_for(duration);
    return true;
}

TEST (test1, test_delaytast) {
    ThreadPool thread_pool(30);
    Task taskminos(std::bind(&minostask, 1, 2));
    thread_pool.AddDelayTask(1000 * 5000, taskminos);
    while (1);
    //EXPECT_EQ(true, test(thread_pool));
}

int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
