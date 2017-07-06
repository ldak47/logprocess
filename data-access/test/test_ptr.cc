#include "gtest/gtest.h"
#include "glog/logging.h"
#include "range_iterator.h"
#include <mutex>
#include <chrono>
#include <thread>
#include <random>
#include <vector>
#include <queue>

class Father {
public:
    Father(){ LOG(INFO) << "father construct.";}
    ~Father(){ LOG(WARNING) << "father destruct.";}
};

class Son : public Father {
public:
    Son(){ LOG(INFO) << "son construct.";}
    ~Son(){ LOG(WARNING) << "son destruct.";}
};

void threadfunc (std::shared_ptr<Father> ptr) {
    std::this_thread::sleep_for(std::chrono::seconds(1));

    static std::mutex mtx;
    std::unique_lock<std::mutex> lock(mtx);
    LOG(INFO) << "thread " << std::this_thread::get_id() << " address: " << ptr.get() << ", use_count: " << ptr.use_count();
}

TEST(test_ptr, test_shared_ptr1) {
    std::shared_ptr<Father> ptr = std::make_shared<Son>();
    LOG(INFO) << "now no thread start, address: " << ptr.get() << ", use_count: " << ptr.use_count();
    std::thread t1(threadfunc, ptr), t2(threadfunc, ptr), t3(threadfunc, ptr);
    ptr.reset();
    LOG(INFO) << "now main reset ptr, address: " << ptr.get() << ", use_count: " << ptr.use_count();

    t1.join();
    LOG(INFO) << "now thread 1 join, address: " << ptr.get() << ", use_count: " << ptr.use_count();
    t2.join();
    LOG(INFO) << "now thread 1 and 2 join, address: " << ptr.get() << ", use_count: " << ptr.use_count();
    t3.join();
    LOG(INFO) << "no ptr point to object Son, now should be destructed already, now all thread exit, address: " << ptr.get() << ", use_count: " << ptr.use_count();
}

/* 用gdb逐步观察(p *(xxx._M_refcount._M_pi)) */
TEST(test_ptr, test_shared_ptr2) {
    std::shared_ptr<int> a = std::make_shared<int>(10);
    EXPECT_EQ(a.use_count(), 1);
    
    std::shared_ptr<int> b = std::make_shared<int>(20);
    EXPECT_EQ(b.use_count(), 1);
    
    std::shared_ptr<int> c(a);
    EXPECT_EQ(a.use_count(), 2);
    EXPECT_EQ(c.use_count(), 2);
    
    std::shared_ptr<int> d = std::make_shared<int>(30);
    EXPECT_EQ(d.use_count(), 1);
    
    std::shared_ptr<int> e(b);
    EXPECT_EQ(b.use_count(), 2);
    EXPECT_EQ(e.use_count(), 2);
    
    a = d;
    EXPECT_EQ(a.use_count(), 2);
    EXPECT_EQ(c.use_count(), 1);
    EXPECT_EQ(d.use_count(), 2);
    
    b = std::make_shared<int>(40);
    EXPECT_EQ(b.use_count(), 1);
    EXPECT_EQ(e.use_count(), 1);
    
    std::shared_ptr<int> f = c;
    EXPECT_EQ(c.use_count(), 2);
    EXPECT_EQ(f.use_count(), 2);
    
    b = f;
    EXPECT_EQ(b.use_count(), 3);
    EXPECT_EQ(c.use_count(), 3);
    EXPECT_EQ(f.use_count(), 3);
    
    LOG(INFO) << "basic exercise success.";
}

TEST(test_ptr, test_shared_ptr3) {
    std::shared_ptr<int> ptr(new int(10));
    EXPECT_EQ(1, ptr.use_count());

    {
        std::shared_ptr<int> p(ptr);
        EXPECT_EQ(2, p.use_count());
        EXPECT_EQ(2, ptr.use_count());
    }
    EXPECT_EQ(1, ptr.use_count());
    std::shared_ptr<int> p(ptr);
    EXPECT_EQ(2, ptr.use_count());
    p.reset();
    EXPECT_EQ(1, ptr.use_count());

    ptr = std::shared_ptr<int>(new int(20));
    EXPECT_EQ(1, ptr.use_count());
}

TEST(test_ptr, test_shared_ptr4) {
    auto a = std::shared_ptr<int>(new int);
    std::shared_ptr<int> b;

    EXPECT_EQ(1, a.unique());
    EXPECT_EQ(0, b.unique());

    b = a;
    EXPECT_EQ(0, a.unique());
    EXPECT_EQ(0, b.unique());

    a.reset();
    EXPECT_EQ(0, a.unique());
    EXPECT_EQ(1, b.unique());
}

/* 底下这个机制, 重在描述shared_ptr的引用计数的特点在COW的一种举例, 不要用多个读线程测试, 多个读线程的COW还需要增加一些同步机制才行 */
TEST(test_ptr, test_shared_ptr_copy_on_write) {
    using gptr = std::shared_ptr<int>;
    gptr ptr(new int(0));
    std::mutex mtx;

    std::vector<std::thread> readers;
    for (int i: common::Range(0, 1)) {
        auto reader = std::thread([&] {
            int curval = 101;
            while (1) {
                gptr p;
                {
                    std::unique_lock<std::mutex> lock(mtx);
                    /* ptr数据空间的引用者增加了本线程的p, 即正在读取
                     * 此时写线程会根据unique发现正在读取, 它会让ptr指向新的数据空间并更新为新数据 */
                    p = ptr;
                }
                
                /* p依然指向ptr的数据空间, 不受写线程ptr指向新数据空间影响, 故这里无需在锁内 */
                if (curval != *p) {
                    LOG(INFO) << "reader " << std::this_thread::get_id() << " get data " << *p;
                    curval = *p;
                }
                std::this_thread::yield();
            }
        });

        readers.push_back(std::move(reader));
    }
    
    auto writer = std::thread([&] {
        while (1) {
            {
                std::unique_lock<std::mutex> lock(mtx);
                if (!ptr.unique()) {
                    /* 发现读线程已经正在读取了, 这时候更新数据, 方式是: ptr指向新的一个空间, 该空间数据和上一把一样
                     * 此时ptr必然unique
                     * 所以本质上, copy-on-write是copy-on-reader-reading */
                    ptr.reset(new int(*ptr));
                } else {
                    /* 发现并没有unique, 即读线程都没有来得及读取当前ptr, 就别再换新数据了
                     * 很多实现没有这个, 写线程不停的更新, 读完全没跟上 */
                    continue;
                }

                /*在新空间更新新的data*/
                assert(ptr.unique());
                std::random_device rd;
                *ptr = rd() % 100;
                LOG(INFO) << "writer, update data to " << *ptr;
            }
            std::this_thread::yield();
        }
    });

    writer.join();
    for (int i: common::Range(0, 1)) {
        readers[i].join();
    }
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
