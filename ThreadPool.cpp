#include <iostream>
#include "RequestInfo.h"
#include "ThreadPool.h"
#include "UDPResponse.h"
#include "commons.h"
#include "utils.h"

template <typename T>
ThreadPool<T>::ThreadPool(T& t):
  myTask(t),
  myCondVar(),
  myMutex(),
  myQueue(),
  myThreads(NUM_THREADS),
  running(false),
  myThrdCounter(0) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Constructing ThreadPool class..." << '\n';
#endif
}

template <typename T>
ThreadPool<T>::~ThreadPool() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Destructing ThreadPool class..." << '\n';
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
    for (auto& thrd: myThreads) { // TODO Check how this loop can be skipped when not necessary (the threads are not running)!
      if (thrd.joinable()) {
        thrd.join();
      }
    }
  }
}

template <typename T>
void ThreadPool<T>::append(const RequestInfo& r) {
  size_t queueSize = 0;
  {
    std::unique_lock<std::mutex> l(myMutex);
    // TODO Add here a proper code block if a queue limit is necessary!
    myQueue.emplace(r);
    queueSize = myQueue.size();
    myCondVar.notify_one();
  }
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Appending request to thread pool" << '\n';
  std::cout << "Current queue size: " << queueSize << '\n';
#endif
}

template <typename T>
void ThreadPool<T>::thread() {
  RequestInfo r(UDP_BUFFER_SIZE);
  unsigned int thrdNum = ++myThrdCounter;
  while (true) {
    {
      std::unique_lock<std::mutex> l(myMutex);
      if (myQueue.empty()) {
        if (running) {
          myCondVar.wait(l);
          continue;
        } else {
#if defined(DEBUG) && VERBOSENESS > 1
          std::cout << getUTCTime() << " [DEBUG] Exiting thread n. " << thrdNum << "..." << '\n';
#endif
          break;
        }
      }
      r = myQueue.front();
      myQueue.pop();
      // TODO Add here a proper code block if a queue limit is necessary!
    }
#if defined(DEBUG) && VERBOSENESS > 2
    std::cout << getUTCTime() << " [DEBUG] Thread n." << thrdNum << " is computing request " << r.referDatagram().token() << "-" << r.referDatagram().tid() << '\n';
#endif
    myTask(r);  // Do the task, passing it the request informations...
  }
}

template class ThreadPool<UDPResponse>;
