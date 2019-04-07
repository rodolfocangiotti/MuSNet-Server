#include <iostream>
#include "RequestInfo.h"
#include "ThreadPool.h"
#include "UDPResponse.h"
#include "commons.h"

template <typename T>
ThreadPool<T>::ThreadPool(T& t):
  myTask(t),
  running(false),
  myCondVar(),
  myMutex(),
  myQueue(),
  myThreads(/*std::thread::hardware_concurrency() - */ 1) { // It seems that a single thread works better than multithreading!
  // TODO Check if code can be optimized to exploit multithreading!
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing ThreadPool class..." << std::endl;
#endif
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing ThreadPool class..." << std::endl;
#endif
  stop();
}

template <typename T>
void ThreadPool<T>::start() {
  std::unique_lock<std::mutex> l(myMutex);
  if (!(running)) {
    for (auto& thrd: myThreads) {
      thrd = std::thread(&ThreadPool<T>::thread, this);
    }
    running = true;
  }
}

template <typename T>
void ThreadPool<T>::stop() {
  bool join = false;
  {
    std::unique_lock<std::mutex> l(myMutex);
    if (running) {
      running = false;
      join = true;
      myCondVar.notify_all();
    }
  }
  if (join) {
    for (auto& thrd: myThreads) { // TODO Check how this loop can be skipped when not necessary (the threads are not running)...
      if (thrd.joinable()) {
        thrd.join();
      }
    }
  }
}

template <typename T>
void ThreadPool<T>::append(const RequestInfo& r) {
  std::unique_lock<std::mutex> l(myMutex);
  // TODO Add here a proper code block if a queue limit is necessary...
  myQueue.emplace(r);
  myCondVar.notify_one();
}

template <typename T>
void ThreadPool<T>::thread() {
  RequestInfo r(UDP_BUFFER_SIZE);
  while (true) {
    {
      std::unique_lock<std::mutex> l(myMutex);
      if (myQueue.empty()) {
        if (running) {
          myCondVar.wait(l);
          continue;
        } else {
          std::cout << "Exiting thread..." << std::endl;
          break;
        }
      }
      r = myQueue.front();
      myQueue.pop();
      // TODO Add here a proper code block if a queue limit is necessary...
    }
    myTask(r);  // Do the task, passing it the request informations...
  }
}

template class ThreadPool<UDPResponse>;
