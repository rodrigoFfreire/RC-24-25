#include "WorkerPool.hpp"

void WorkerPool::workerThread() {
    while (1) {
        std::function<void()> connectionHandler;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            alertCond.wait(lock, [this] { 
                return !connectionQueue.empty() || isStopping; 
            });

            if (isStopping && connectionQueue.empty()) {
                return;  // Exit thread
            }

            connectionHandler = connectionQueue.front();
            connectionQueue.pop();
        }
        connectionHandler();  // Execute the work
    }
}

WorkerPool::WorkerPool(size_t num_workers)
    : isStopping(false) {
    for (size_t i = 0; i < num_workers; ++i) {
        workers.emplace_back(&WorkerPool::workerThread, this);
    }
}

WorkerPool::~WorkerPool()
{
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        isStopping = true;
    }
    alertCond.notify_all();

    for (std::thread &worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void WorkerPool::enqueueConnection(std::function<void()> connHandler) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        connectionQueue.push(connHandler);
    }
    alertCond.notify_one();
}
