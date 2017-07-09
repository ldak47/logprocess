#ifndef DATAACCESS_COMMON_NONCOPYABLE_H_
#define DATAACCESS_COMMON_NONCOPYABLE_H_

namespace common {

class noncopyable {
protected:
    constexpr noncopyable() = default;
    ~noncopyable() = default;
    noncopyable(const noncopyable &) = delete;
    noncopyable &operator= (const noncopyable &) = delete;
};

};
#endif
