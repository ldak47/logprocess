#include "glog/logging.h"
#include "gtest/gtest.h"
#include <cmath>
#include <limits.h>

constexpr int calc_in_compile_0 () {
    return 100;
}

constexpr int calc_in_compile_1 (int a) {
    return a * 100;
}

constexpr int calc_in_compile_2 (int b, int c) {
    return c * calc_in_compile_1(b);
}

//近似const, 不同的是constexpr生效于编译时而不是运行时, 重点应用于修饰函数使其在编译期大幅度被解释.
//我们可以声明某些函数是常量表达式, 这样的函数必须在编译期间计算出它们的值, 必须满足以下条件:
//1. 返回值和参数必须是字母值类型(Literal)
//2. 函数体必须只包含一个return语句
//3. 函数提可以包含其他的语句，但是这些语句不能在运行期起作用
//4. 函数可以不返回常量，但是在调用的时候实参必须传入常量表达式

TEST(test_constexpr, test1) {
    EXPECT_EQ(100, calc_in_compile_0());
    constexpr int a = 1;
    EXPECT_EQ(100, calc_in_compile_1(a));
    EXPECT_EQ(10000, calc_in_compile_2(a, calc_in_compile_1(a)));
}

TEST(test_constexpr, test2) {
class Calc {
    double a_;
public:
    /*构造函数必须用constexpr修饰*/
    constexpr Calc(double a):a_(a) {}

    constexpr double GetFabs() const {
        return std::fabs(a_);
    }
    constexpr double GetAbs() const {
        return std::abs(a_);
    }
    constexpr double GetSquare() const {
        return a_ * a_;
    }
};

    constexpr Calc calc(5.1);
    constexpr double _fabs = calc.GetFabs();
    ///_fabs = 10.0;
    LOG(INFO) << "fabs: " << _fabs;
    double _abs = calc.GetAbs();
    LOG(INFO) << "abs: " << _abs;
    _abs = 10.0;
    LOG(INFO) << "abs: " << _abs;
    double _square = calc.GetSquare();
    LOG(INFO) << "square: " << _square;
    _square = 10.0;
    LOG(INFO) << "square: " << _square;
}

/*代替"const _max = INT_MAX". 在这里貌似不明显, 在更复杂地方显出用途*/
static constexpr int max () {
    return INT_MAX;
}

static constexpr int min () {
    return INT_MIN;
}

TEST(test_constexpr, test3) {
    constexpr int _max = max(), _min = min();
    LOG(INFO) << "max: " << _max;
    LOG(INFO) << "min: " << _min;
}

int main (int argc, char *argv[]) {
    ::google::SetStderrLogging(google::GLOG_FATAL);
    FLAGS_logbufsecs = 0;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
