#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#ifdef __linux__
#include <condition_variable>
#endif
#include <queue>
#include <thread>
#include <vector>
#include "RequestInfo.h"

template <typename T>
class ThreadPool {
public:
  ThreadPool(T& t);
  ~ThreadPool();
  // ********************
  void start();
  void stop();
  void append(const RequestInfo& r);
private:
  void thread();
  // ********************
  T& myTask;
  // ********************
  std::condition_variable myCondVar;
  std::mutex myMutex;
  std::queue<RequestInfo> myQueue;
  std::vector<std::thread> myThreads;
  bool running;
  // ********************
  unsigned int myThrdCounter;
};

#endif
