#include "glog/logging.h"
#include "gtest/gtest.h"
#include "boost/noncopyable.hpp"

class NonCopyAbleClass_cpp11 {
    int obj_;
public:
    NonCopyAbleClass_cpp11(int obj): obj_(obj){}
    NonCopyAbleClass_cpp11(const NonCopyAbleClass_cpp11 &instance) = delete;
    NonCopyAbleClass_cpp11 &operator=(const NonCopyAbleClass_cpp11 &instance) = delete;
};

class NonCopyAbleClass_boost : public boost::noncopyable {
    int obj_;
public:
    NonCopyAbleClass_boost(int obj): obj_(obj){}
};

TEST(test_noncopyable, test_boost) {
    NonCopyAbleClass_boost ins1(1);
    //NonCopyAbleClass_boost ins2(ins1);//could not make
    //NonCopyAbleClass_boost ins2 = ins1;//could not make
}

TEST(test_noncopyable, test_cpp11) {
    NonCopyAbleClass_cpp11 ins1(1);
    //NonCopyAbleClass_cpp11 ins2(ins1);//could not make
    //NonCopyAbleClass_cpp11 ins2 = ins1;//could not make
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
