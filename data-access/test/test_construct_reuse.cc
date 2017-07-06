#include "gtest/gtest.h"
#include "glog/logging.h"


TEST(test_construct_reuse, test) {
    struct A {
        bool a_;
        char b_;
        int c_;
        float d_;
        double e_;
        A(bool a, char b, int c, float d, double e): a_(a), b_(b), c_(c), d_(d), e_(e) {}
        //construct reuse
        A (int c): A(true, 'b', c, 1.1, 1000.1) {}
        A (double e): A (false, 'a', 0, 0.1, e) {}
    };

    A o1(10);
    LOG(INFO) << "a: " << o1.a_ << ", b: " << o1.b_ << ", c: " << o1.c_ << ", d: " << o1.d_ << ", e: " << o1.e_;
    A o2(5.5);
    LOG(INFO) << "a: " << o2.a_ << ", b: " << o2.b_ << ", c: " << o2.c_ << ", d: " << o2.d_ << ", e: " << o2.e_;
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
