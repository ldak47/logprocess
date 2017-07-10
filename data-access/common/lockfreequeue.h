#include <atomic>


template<class T> class LFQ {
    int _capacity;
    std::atomic<int> head;
    std::atomic<int> tail;
    typedef struct item {
        bool readable;
        T value;
    }item_t;
    item_t *itemlist;
public:
    explicit LFQ(int capacity);
    ~LFQ();
    bool try_push(const T &value);
    bool try_pop(T &value);
};
