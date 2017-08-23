#include "skiplist_funcs.h"
#include "gtest/gtest.h"
#include "glog/logging.h"
#include "range_iterator.h"
#include <sys/time.h>
#include <memory.h>


TEST(test, skiplist) {
    std::unique_ptr<common::Skiplist<time_t, int>> sk;
    sk.reset(new common::Skiplist<time_t, int>());

    size_t cnt;
    std::vector<int> res;
    int val;
    sk->Modify(10, 10);
    LOG(INFO) << sk->Search(10, val);
    LOG(INFO) << "res: " << val;
    sk->Modify(11, 11);
    sk->Modify(1, 2);
    LOG(INFO) << sk->RangeGet(-1, 3, res);
    for (auto r: res) {
        LOG(INFO) << r;
    }
    res.clear();
    sk->Modify(40, 40);
    sk->Modify(5, 5);
    sk->Remove(5);
    sk->Modify(7, 7);
    sk->Remove(7);
    sk->Modify(90, 90);

    LOG(INFO) << sk->RangeCount(-5, 10, cnt);
    LOG(INFO) << "cntcnt: " << cnt;
    LOG(INFO) << sk->RangeGet(5, 7, res);
    for (auto r: res) {
        LOG(INFO) << r;
    }
    LOG(INFO) << sk->RangeCount(7, 11, cnt);
    LOG(INFO) << "cnt: " << cnt;
    LOG(INFO) << sk->RangeCount(7, 10, cnt);
    LOG(INFO) << "cnt: " << cnt;
    LOG(INFO) << sk->RangeCount(5, 11, cnt);
    LOG(INFO) << "cnt: " << cnt;
    LOG(INFO) << sk->RangeCount(5, 10, cnt);
    LOG(INFO) << "cnt: " << cnt;

    LOG(INFO) << sk->RangeGet(7, 11, res);
    for (auto r: res) {
        LOG(INFO) << r;
    }
    LOG(INFO) << "#####";
    LOG(INFO) << sk->RangeGet(7, 10, res);
    for (auto r: res) {
        LOG(INFO) << r;
    }
    LOG(INFO) << "#####";
    LOG(INFO) << sk->RangeGet(5, 11, res);
    for (auto r: res) {
        LOG(INFO) << r;
    }
    LOG(INFO) << "#####";
    LOG(INFO) << sk->RangeGet(5, 10, res);
    for (auto r: res) {
        LOG(INFO) << r;
    }
    LOG(INFO) << "#####";

    sk->Search(11, val);
    LOG(INFO) << val;
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
