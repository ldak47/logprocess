#include "gtest/gtest.h"
#include "glog/logging.h"
#include <string>

//typedef std::tuple<bool, char, int, float, double> res_tp;
using res_tp = std::tuple<bool, char, int, float, double>;

res_tp Function1 () {
    res_tp result;
    res_tp tmpres(true, 'b', 11, 1.1, 100.1);
    result = tmpres;
    return result;
}

res_tp Function2 () {
    res_tp result;
    res_tp tmpres(true, 'b', 2, 2.1, 10000.1);
    result = tmpres;
    return result;
}

TEST(test_tuple, test) {
    /*c++11的元组, 彻底抹平了和python的"返回多个返回值"的差距*/
    res_tp res = Function1();
    LOG(INFO) << "res.bool: " << std::get<0>(res);
    LOG(INFO) << "res.char: " << std::get<1>(res);
    LOG(INFO) << "res.int: " << std::get<2>(res);
    LOG(INFO) << "res.float: " << std::get<3>(res);
    LOG(INFO) << "res.double: " << std::get<4>(res);

    /*tuple支持比较运算符, 比较方式为元组内元素依次比较*/
    res_tp res_other = Function2();
    LOG(INFO) << "res > res_other?: " << (res > res_other);
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

