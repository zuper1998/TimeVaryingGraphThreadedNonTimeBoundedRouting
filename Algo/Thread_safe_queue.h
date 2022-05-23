//
// Created by Narcano on 5/21/2022.
//

#ifndef TVGGRAPHCALC_THREAD_SAFE_QUEUE_H
#define TVGGRAPHCALC_THREAD_SAFE_QUEUE_H
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>
using namespace  std::chrono_literals;

template <typename T>
class Thread_safe_queue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
public:


    void push(T const& t){
        std::unique_lock l(mutex_);
        queue_.push(t);
        cond_.notify_all();
    }
    void push(T &&t){
        std::unique_lock l(mutex_);
        queue_.push(t);
        cond_.notify_all();
    }
    void emplace(T &&t){
        std::unique_lock l(mutex_);
        queue_.template emplace(t);
        cond_.notify_all();
    }


    T pop(){
        std::unique_lock l(mutex_);
        //Long time
        cond_.wait(l,[this]{return !queue_.empty();});
        T ret = queue_.front();
        queue_.pop();
        return ret;
    }

    void GenDone(){
        //This is a hack
        for(int i = 0 ; i < 50; i++){
            queue_.push(T());
        }
        cond_.notify_all();
    }

    [[nodiscard]]bool empty() const {
        return queue_.empty();
    }
};


#endif //TVGGRAPHCALC_THREAD_SAFE_QUEUE_H
