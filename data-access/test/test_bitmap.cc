#include "gtest/gtest.h"
#include "glog/logging.h"
#include "bitmap.h"

TEST (test, bitmap) {
    common::Bitmap bitmap1(0), bitmap2(0);
    bitmap2.set(0);
    bitmap2.set(1);
    bitmap2.set(2);
    bitmap2.set(3);
    bitmap2.set(4);
    bitmap2.set(5);
    bitmap2.set(29);
    bitmap2.set(131);
    bitmap1.set(0);
    bitmap1.set(1);
    bitmap1.set(2);
    bitmap1.set(3);
    bitmap1.set(4);
    bitmap1.set(5);
    bitmap1.set(29);
    bitmap1.set(131);

    LOG(INFO) << bitmap2.get(0);
    LOG(INFO) << bitmap2.get(1);
    LOG(INFO) << bitmap2.get(2);
    LOG(INFO) << bitmap2.get(5);
    LOG(INFO) << bitmap2.get(27);
    LOG(INFO) << bitmap2.get(131);
    LOG(INFO) << bitmap2.get(10);
    LOG(INFO) << bitmap1.get(0);
    LOG(INFO) << bitmap1.get(1);
    LOG(INFO) << bitmap1.get(2);
    LOG(INFO) << bitmap1.get(5);
    LOG(INFO) << bitmap1.get(27);
    LOG(INFO) << bitmap1.get(131);
    LOG(INFO) << bitmap1.get(10);
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
