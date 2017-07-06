#include "gtest/gtest.h"
#include "glog/logging.h"


/*static_assert是用于涉及模板的assert, 编译期就能发现不满足的情况.
 * 同理, 所assert的条件必须也是编译器可以得到的(如sizeof).*/
template<class T> class C {
    T data1_;
    int data2_;
public:
    C(T data1, int data2):data1_(data1), data2_(data2) {
        /*if the condition is not satisfiedm, would be errored by compiler in compling*/
        //static_assert(sizeof(T) > 4, "sizeof(T) is not larger than 4");
        static_assert(sizeof(T) >= 4, "sizeof(T) is not larger than 4");
        //static_assert(data2_ >= 10, "could not use static_assert here! condition must could be calced in compling!");
    }
};

TEST(test_static_assert, test) {
    C<double> c(1.1, 1);
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
