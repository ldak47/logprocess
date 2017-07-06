#include "thread_pool.h"
#include "range_iterator.h"


ThreadPool::ThreadPool (int thread_num): thread_num_(thread_num),
                                         pending_num_(0), 
                                         running_num_(0), 
                                         task_count_(0), 
                                         stop_(true) {
    Start();
}

ThreadPool::~ThreadPool () {
    Stop(false);
}

bool ThreadPool::Start () {
    std::unique_lock<std::mutex> lock(mtx_);
    stop_ = false;
    for (auto i: common::Range(0, thread_num_)) {
        ths_.push_back(std::thread(&ThreadFunc, this));
    }
}

bool ThreadPool::Stop (bool graceful) {
    if (graceful) {
        while (pending_num_) {
            std::chrono::milliseconds duration(5000);
            std::this_thread::sleep_for(duration);
        }
    }

    stop_ = true;
    cond_.notify_all();
    for (auto i: common::Range(0, thread_num_)) {
        ths_[i].join();
    }

    pending_num_ = running_num_ = task_count_ = 0;
}

void ThreadPool::AddTask (const Task &task) {
    std::unique_lock<std::mutex> lock(mtx_);
    queue_.push_back(task);
    ++pending_num_;
    ++task_count_;
    cond_.notify_one();
}

void ThreadPool::AddPriorityTask (const Task &task) {
    std::unique_lock<std::mutex> lock(mtx_);
    queue_.push_front(task);
    ++pending_num_;
    ++task_count_;
    cond_.notify_one();
}

void ThreadPool::AddDelayTask (int timeval, const Task &task) {
    std::unique_lock<std::mutex> lock(mtx_);
    Timertask newtask(task, timeval);
    timer_queue_.push(newtask);
    ++task_count_;
    cond_.notify_one();
}

void ThreadPool::AddTimerTask (int timeval, const Task &task) {
    std::unique_lock<std::mutex> lock(mtx_);
    Timertask newtask(task, timeval, true);
    timer_queue_.push(newtask);
    ++task_count_;
    cond_.notify_one();
}

void ThreadPool::process () {
    while (1) {
        std::unique_lock<std::mutex> lock(mtx_);
        while (timer_queue_.empty() && queue_.empty() && !stop_) {
            cond_.wait(lock);
        }

        if (stop_) {
            break;
        }

        if (!timer_queue_.empty()) {
            int64_t nowtime = common::getime_micros();
            Timertask newestask = timer_queue_.top();
            if (newestask.exec_time_ <= nowtime) {
                timer_queue_.pop();
                Task task = newestask.task_;
                bool flag = newestask.flag_;
                int64_t timeval = newestask.timeval_;
                if (flag) {
                    Timertask newtask(task, timeval, true);
                    timer_queue_.push(newtask);
                    ++task_count_;
                }

                ++running_num_;
                --pending_num_;
                lock.unlock();
                task();
                lock.lock();
                --running_num_;
            }
        }

        if (!queue_.empty()) {
            Task task = queue_.front();
            queue_.pop_front();
            
            --pending_num_;
            ++running_num_;
            lock.unlock();
            task();
            lock.lock();
            --running_num_;
        }
    }
}
