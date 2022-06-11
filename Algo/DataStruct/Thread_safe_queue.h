//
// Created by Narcano on 5/21/2022.
//

#ifndef TVGGRAPHCALC_THREAD_SAFE_QUEUE_H
#define TVGGRAPHCALC_THREAD_SAFE_QUEUE_H
#include <queue>
#include <mutex>
#include <shared_mutex>
#include <atomic>
#include <condition_variable>
using namespace  std::chrono_literals;

template <typename T>
class Thread_safe_queue {
    std::queue<T> queue_;
    mutable std::mutex mutex_;
    std::condition_variable cond_;
    std::atomic_bool finished = false;
    size_t max_size=0;
public:
    //For memory usage
    explicit Thread_safe_queue(size_t max_size): max_size(max_size)  {

    }


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
    [[nodiscard]]
    size_t size() const{
        std::unique_lock l(mutex_);
        return  queue_.size();
    }

    T pop(){

        std::unique_lock l(mutex_);
        //if done
        if(queue_.empty() && finished.load()){
            return {};
        }

        //Long time
        cond_.wait(l,[this]{return !queue_.empty();});
        T ret = queue_.front();
        queue_.pop();
        return ret;
    }

    void GenDone(){
        finished.store(true);
        //This is a hack
        cond_.notify_all();
    }

    [[nodiscard]]bool empty() const {
        return queue_.empty();
    }
};


#endif //TVGGRAPHCALC_THREAD_SAFE_QUEUE_H
