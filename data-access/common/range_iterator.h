#ifndef COMMON_RANGE_ITERATOR_H
#define COMMON_RANGE_ITERATOR_H

namespace common {

class Fakeiter {
public:
    explicit Fakeiter (long value): value_(value) {}
    bool operator!= (const Fakeiter &other) const {
        return (GetValue() != other.GetValue());
    }
    long operator* () const {
        return GetValue();
    }
    const Fakeiter &operator++ () {
        ++value_;
        return *this;
    }
    
private:
    long GetValue () const {
        return value_;
    }
    long value_;
};

class Range {
public:
    Range(long begin, long end):begin_(begin), end_(end) {}
    Fakeiter begin() {
        return Fakeiter(begin_);
    }
    Fakeiter end() {
        return Fakeiter(end_);
    }
    
private:
    long begin_;
    long end_;
};

};
#endif
