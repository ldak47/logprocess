#include "glog/logging.h"
#include "gtest/gtest.h"
#include <functional>
#include <string>

/*lambda的类型: std::function*/
TEST(test_function, test_simple_lambda) {
    int a = 1, b = 2;
    auto multi = [](int a, int b){
        b = a + a + a;
        return a + b;
    };

    LOG(INFO) << "by lambda: " << multi(a, b);
}

int func1 (int a, int b) {
    b = a + a + a;
    return a + b;
}

int func2 (int a, double b, std::string c) {
    b = a + a + a;
    return int(a + b);
}

int func3 (auto f) {
    return f(1, 2);
}

int func4 (const int &a, int &b) {
    b = 3;
    return a + b;
}

int func5 (int *a) {
    return *a;
}

TEST(test_function, test_simple_function) {
    /*可以由函数指针直接构造*/
    auto a = 1, b = 2;
    std::function<int (int, int)> modify_add0(func1);
    LOG(INFO) << "directly assign function: " << modify_add0(a, b);
    
    /*可以由std::bind形成的函数指针构造*/
    std::function<int (int, int)> modify_add1(std::bind(func1, std::placeholders::_1, std::placeholders::_2));
    auto auto1 = std::bind(func1, std::placeholders::_1, std::placeholders::_2);
    LOG(INFO) << "by bind: " << modify_add1(a, b);
    LOG(INFO) << "run auto in function: " << func3(auto1);
    LOG(INFO) << "directly run auto: " << auto1(a, b);

    /*std::function内的定义了该function调用时的顺序, 也是_1、_2、..._n的顺序, bind内要整理符合绑定的函数参数顺序*/
    std::function<int (std::string, int)> modify_add2 = std::bind(func2, std::placeholders::_2, 2.0, std::placeholders::_1);
    LOG(INFO) << "by bind with partly arg: " << modify_add2("aaa", 1);
        
    /*直接定义函数体的方式(可以由一个lambda构造)*/
    //std::function<int ()> modify_add3 = std::function<int ()>([a, &b]{
    std::function<int ()> modify_add3 = std::function<int ()>([=, &b]{
        b = a + a + a;
        return a + b;
    });
    LOG(INFO) << "directly in-function: " << modify_add3();

    std::function<int (const int&, int&)> modify_add4 = std::bind(func4, std::placeholders::_1, std::placeholders::_2);
    LOG(INFO) << "args is const reference and reference: " << modify_add4(std::cref(a), std::ref(a));
    std::function<int (int *)> modify_add5 = std::bind(func5, std::placeholders::_1);
    LOG(INFO) << "args is const reference and reference: " << modify_add5(&a);
}
 
TEST(test_function, test_class_function) {
    class ca {
    public:
        bool func(int a) {
            LOG(INFO) << "aaa: " << a;
        }
    };

    ca o;
    std::function<bool (int)> f = std::bind(&ca::func, o, std::placeholders::_1);
    f(1);
    std::vector<std::function<bool (int *)>> chk_;
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(::google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
