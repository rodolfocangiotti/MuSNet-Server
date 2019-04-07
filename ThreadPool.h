#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <queue>
#include <thread>
#include <vector>
#include "RequestInfo.h"

template <typename T>
class ThreadPool {
public:
  ThreadPool(T& t);
  ~ThreadPool();
  void start();
  void stop();
  void append(const RequestInfo& r); // TODO Check if a copy of the object is better than a reference...
private:
  void thread();
  T& myTask;
  bool running;
  std::condition_variable myCondVar;
  std::mutex myMutex;
  std::queue<RequestInfo> myQueue;
  std::vector<std::thread> myThreads;
};

#endif
