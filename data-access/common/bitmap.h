#ifndef DATAACCESS_COMMON_BITMAP_H
#define DATAACCESS_COMMON_BITMAP_H
#include <vector>

namespace common {

class Bitmap {
    int n;
    const int mask;
    const int shift;
    const unsigned int pos;
    std::vector<unsigned int> a;
    
public:
    Bitmap(int num):n(num), mask(0x1F), shift(5), pos(1 << mask), a(1 + n/32, 0) {}
    
    void set(int i) {
        a[i>>shift] |= (pos>>(i & mask));
    }
    int get(int i) {
        return a[i>>shift] & (pos>>(i & mask));
    }
    void clr(int i) {
        a[i>>shift] &= ~(pos>>(i & mask));
    }
};
    
};
#endif
