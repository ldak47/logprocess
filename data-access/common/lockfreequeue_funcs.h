#include "lockfreequeue.h"
#include "glog/logging.h"
#include <thread>

template<class T> LFQ<T>::LFQ (int capacity): _capacity(capacity), head(0), tail(0) {
    itemlist = new item[_capacity];
    memset(itemlist, 0, sizeof(item) * _capacity);
}

template<class T> LFQ<T>::~LFQ () {
    delete []itemlist;
    itemlist = 0;
}

template<class T> bool LFQ<T>::try_push (const T &value) {
    while (1) {
    //保证本次对tail的读操作, 是对tail已有的读操作都完成之后再进行的. head同理
        int tail_ = tail.load(std::memory_order_acquire);
        int new_tail = tail_ + 1;
        int head_ = head.load(std::memory_order_acquire);

        if (new_tail - head_ >= _capacity) {
            //LOG(INFO) << "new_tail is " << new_tail << ", head_ is " << head_ << ", upper than _capacity " << _capacity;
            return false;
        }

    //替换tail由tail_为new_tail, 并在new_tail处添加新的value和可读标志
        if (tail.compare_exchange_strong(tail_, new_tail, std::memory_order_acq_rel)) {
        LOG(INFO) << "tid" << std::this_thread::get_id() << " push " << tail_ << " of " << tail_%_capacity << " with value " << value;
            tail_ %= _capacity;

            std::atomic_thread_fence(std::memory_order_acquire);
            while(itemlist[tail_].readable);
            itemlist[tail_].value = value;
            
            std::atomic_thread_fence(std::memory_order_release);
            itemlist[tail_].readable = true;
            return true;
        }
    }
}

template<class T> bool LFQ<T>::try_pop (T &value) {
    while (1) {
    //确保获取的tail和head的读操作, 是最新的读操作
        int head_ = head.load(std::memory_order_acquire);
        int tail_ = tail.load(std::memory_order_acquire);

        if (head_ == tail_) {
            LOG(INFO) << "tid" << std::this_thread::get_id() << " current tail_ is " << tail_ << ", current head is " << head_ << ", head == tail";
            return false;
        }

    //设置head前移
    //如果成功则读旧head的value并修改标志位
        int new_head = head_ + 1;
        if (head.compare_exchange_strong(head_, new_head, std::memory_order_acq_rel)) {
            int rawhead = head_;
            head_ %= _capacity;

            std::atomic_thread_fence(std::memory_order_acquire);
            while(!itemlist[head_].readable);
            value = itemlist[head_].value;
            
            std::atomic_thread_fence(std::memory_order_release);
            itemlist[head_].readable = false;
        LOG(INFO) << "tid" << std::this_thread::get_id() << " pop " << rawhead << " of " << head_ << " with " << value;
            return true;
        }
    }
}
