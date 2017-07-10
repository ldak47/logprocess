#include "gtest/gtest.h"
#include "lockfreequeue_funcs.h"
#include <string>
#include <thread>
#include <chrono>
#include "boost/lockfree/queue.hpp"

bool test (LFQ<int> &lfq) {
    const int NUM_PRODUCERS = 64;
    const int NUM_CONSUMERS = 2;
    const int NUM_ITERATIONS = 512;

    bool table[NUM_PRODUCERS * NUM_ITERATIONS];
    memset(table, 0, sizeof(bool) * NUM_PRODUCERS * NUM_ITERATIONS);

    std::vector<std::thread> threads(NUM_PRODUCERS + NUM_CONSUMERS);
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();

    std::atomic<int> pop_count(NUM_PRODUCERS * NUM_ITERATIONS);
    std::atomic<int> push_count(0);

//每个生产者线程(共NUM_PRODUCERS个), 做NUM_ITERATIONS次插入的操作, 依次插入0-NUM_ITERATIONS、NUM_ITERATIONS-NUM_ITERATIONS*2、......NUM_ITERATIONS * NUM_PRODUCERS-NUM_ITERATIONS * NUM_PRODUCERS + NUM_ITERATIONS
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        threads[i] = std::thread([&lfq, i, &push_count](){
            int base = i * NUM_ITERATIONS;
        //向lfq依次插入(i * NUM_ITERATIONS + 0)到(i * NUM_ITERATIONS + NUM_ITERATIONS - 1)
            for (int j = 0; j < NUM_ITERATIONS; j++) {
                while(!lfq.try_push(base + j));
                push_count.fetch_add(1);
            }
        });
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        threads[i + NUM_PRODUCERS] = std::thread([&lfq, &pop_count, &table](){
            int v;
            while (pop_count.load() > 0) {
                if (lfq.try_pop(v)) {
                    if (table[v]) {
                        LOG(ERROR) << v << " already exists" << std::endl;
                    }
                    table[v] = true;
                    pop_count.fetch_sub(1);
                }
            }
        });
    }

    for (int i = 0; i < NUM_PRODUCERS + NUM_CONSUMERS; i++) {
        threads[i].join();
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    LOG(INFO) << "duration is " << duration.count() << std::endl;
    
    std::atomic_thread_fence(std::memory_order_acq_rel);
    bool result = true;
    for (int i = 0; i < NUM_PRODUCERS * NUM_ITERATIONS; i++) {
        if (!table[i]) {
            LOG(FATAL) << "failed at " << i << std::endl;
            result = false;
        }
    }

    LOG(INFO) << "result: " << result << std::endl;
    return result;
}

bool test_boost (boost::lockfree::queue<int> *lfq) {
    const int NUM_PRODUCERS = 64;
    const int NUM_CONSUMERS = 64;
    const int NUM_ITERATIONS = 512;

    bool table[NUM_PRODUCERS * NUM_ITERATIONS];
    memset(table, 0, sizeof(bool) * NUM_PRODUCERS * NUM_ITERATIONS);

    std::vector<std::thread> threads(NUM_PRODUCERS + NUM_CONSUMERS);
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();

    std::atomic<int> pop_count(NUM_PRODUCERS * NUM_ITERATIONS);
    std::atomic<int> push_count(0);

//每个生产者线程(共NUM_PRODUCERS个), 做NUM_ITERATIONS次插入的操作, 依次插入0-NUM_ITERATIONS、NUM_ITERATIONS-NUM_ITERATIONS*2、......NUM_ITERATIONS * NUM_PRODUCERS-NUM_ITERATIONS * NUM_PRODUCERS + NUM_ITERATIONS
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        threads[i] = std::thread([lfq, i, &push_count](){
            int base = i * NUM_ITERATIONS;
        //向lfq依次插入(i * NUM_ITERATIONS + 0)到(i * NUM_ITERATIONS + NUM_ITERATIONS - 1)
            for (int j = 0; j < NUM_ITERATIONS; j++) {
                while(!lfq->push(base + j));
                push_count.fetch_add(1);
                LOG(INFO) << "push " << base + j;
            }
        });
    }

    for (int i = 0; i < NUM_CONSUMERS; i++) {
        threads[i + NUM_PRODUCERS] = std::thread([lfq, &pop_count, &table](){
            int v;
            while (pop_count.load() > 0) {
                if (lfq->pop(v)) {
                    if (table[v]) {
                        LOG(FATAL) << v << " already exists" << std::endl;
                    }
                    table[v] = true;
                    pop_count.fetch_sub(1);
                    LOG(INFO) << "pop " << v;
                }
            }
        });
    }

    for (int i = 0; i < NUM_PRODUCERS + NUM_CONSUMERS; i++) {
        threads[i].join();
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> duration = end - start;
    LOG(INFO) << "duration is " << duration.count() << std::endl;
    
    std::atomic_thread_fence(std::memory_order_acq_rel);
    bool result = true;
    for (int i = 0; i < NUM_PRODUCERS * NUM_ITERATIONS; i++) {
        if (!table[i]) {
            LOG(FATAL) << "failed at " << i << std::endl;
            result = false;
        }
    }

    LOG(INFO) << "result: " << result << std::endl;
    return result;   
}
//还是boost的好使
TEST(test2, test_boost) {
    boost::lockfree::queue<int> *lfq = new boost::lockfree::queue<int>(1000);
    EXPECT_EQ(true, test_boost(lfq));
    delete lfq;
}
/*
TEST(test1, test) {
    LFQ<int> lfq(100);
    EXPECT_EQ(true, test(lfq));
}
*/
int main (int argc, char *argv[]) {
    google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
