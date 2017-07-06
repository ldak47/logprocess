#ifndef COMMOM_THREAD_POOL_H
#define COMMOM_THREAD_POOL_H
#include "util.h"
#include <glog/logging.h>

typedef std::function<void ()> Task;
struct Timertask {
    bool flag_;
    Task task_;
    int64_t timeval_;
    int64_t exec_time_;
    bool operator< (const struct Timertask otherone) const {
        return exec_time_ > otherone.exec_time_;
    }
    Timertask(const Task &task, int64_t timeval, int64_t exec_time, bool flag = false):flag_(flag), task_(task), timeval_(timeval), exec_time_(exec_time) {}
    Timertask(const Task &task, int64_t timeval, bool flag = false):flag_(flag), task_(task), timeval_(timeval) {
        int64_t nowtime = common::getime_micros();
        exec_time_ = timeval_ + nowtime;
    }
};
    
class ThreadPool {
private:
    std::atomic<uint64_t> pending_num_;
    std::atomic<uint64_t> running_num_;
    uint64_t task_count_;
    
    bool stop_;
    int thread_num_;
    std::vector<std::thread> ths_;
    
    std::mutex mtx_;
    std::condition_variable cond_;
    std::deque<Task> queue_;
    std::priority_queue<Timertask> timer_queue_;
    
public:
    ThreadPool(int thread_num);
    ~ThreadPool();
    bool Start();
    bool Stop(bool graceful);

    void AddTask(const Task &task);
    void AddPriorityTask(const Task &task);
    void AddDelayTask(int timeval, const Task &task);
    void AddTimerTask(int timeval, const Task &task);
    bool IsEmpty() {return (running_num_ > 0)?false:true;}
    bool CancelTask();

    static void *ThreadFunc(void *arg) {
        reinterpret_cast<ThreadPool *>(arg)->process();
        return 0;
    }
    void process();
};
#endif
