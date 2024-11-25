#ifndef SERVER_WORKER_POOL
#define SERVER_WORKER_POOL

#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>


class WorkerPool {
private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> connectionQueue;
    std::mutex queueMutex;
    std::condition_variable alertCond;
    bool isStopping;

    void workerThread();

public:
    WorkerPool(size_t num_workers);
    ~WorkerPool();

    void enqueueConnection(std::function<void()> connHandler);
};

#endif