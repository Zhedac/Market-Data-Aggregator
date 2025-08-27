#include "thread_pool.hpp"
#include <algorithm>
#include <future>
#include <iostream>
#include <queue>
#include <thread>
#include <vector>
using namespace std;

ThreadPool::ThreadPool(size_t numThreads) : stop(false) {
    for (size_t i = 0; i < numThreads; i++) {
        workers.emplace_back([this] {
            for (;;) {
                function<void()> task;

                {
                    unique_lock<mutex> lock(this->queueMutex);
                    this->condition.wait(lock, [this] {
                        return this->stop || !this->tasks.empty();
                    });
                    if (this->stop && this->tasks.empty()) {
                        return;
                    }
                    task = move(this->tasks.front());
                    this->tasks.pop();
                }

                task();
            }
        });
    }
}



ThreadPool::~ThreadPool() {
    {
        unique_lock<mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (thread& worker : workers) {
        worker.join();
    }
}