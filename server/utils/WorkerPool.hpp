#ifndef SERVER_WORKER_POOL
#define SERVER_WORKER_POOL

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class WorkerPool {
 private:
  std::vector<std::thread> workers;
  std::queue<std::function<void()>> connectionQueue;
  std::mutex queueMutex;
  std::condition_variable alertCond;
  bool isStopping = false;

  void workerThread();

 public:
  void dispatch(size_t num_workers);
  ~WorkerPool();
  void enqueueConnection(std::function<void()> connHandler);
};

#endif