#include "WorkerPool.hpp"

#include <iostream>

/// @brief TCP worker thread, pops a connection request from the queue and executes it
void WorkerPool::workerThread() {
  while (1) {
    std::function<void()> connectionHandler;
    {
      std::unique_lock<std::mutex> lock(queueMutex);
      alertCond.wait(lock, [this] { return !connectionQueue.empty() || isStopping; });

      if (isStopping && connectionQueue.empty()) {
        return;  // Exit thread
      }

      connectionHandler = connectionQueue.front();
      connectionQueue.pop();
    }
    connectionHandler();  // Execute the work
  }
}

/// @brief Dispatches `num_workers` worker threads
/// @param num_workers
void WorkerPool::dispatch(size_t num_workers) {
  for (size_t i = 0; i < num_workers; ++i) {
    workers.emplace_back(&WorkerPool::workerThread, this);
  }
}

/// @brief WorkerPool destructor, terminates the worker threads
WorkerPool::~WorkerPool() {
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    isStopping = true;
    alertCond.notify_all();
  }

  for (std::thread &worker : workers) {
    if (worker.joinable()) {
      worker.join();
    }
  }
}

/// @brief Enqueues a TCP connection request to be handled as soon as possible
/// @param connHandler
void WorkerPool::enqueueConnection(std::function<void()> connHandler) {
  {
    std::lock_guard<std::mutex> lock(queueMutex);
    connectionQueue.push(connHandler);
    alertCond.notify_one();
  }
}
