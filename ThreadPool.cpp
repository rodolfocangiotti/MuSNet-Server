#include <iostream>
#include "Console.h"
#include "TCPRequestInfo.h"
#include "TCPResponse.h"
#include "ThreadPool.h"
#include "UDPRequestInfo.h"
#include "UDPResponse.h"
#include "commons.h"
#include "utils.h"

template <typename T, typename R>
ThreadPool<T, R>::ThreadPool(T& t, uint numThreads):
  myTask(t),
  myCondVar(),
  myMutex(),
  myQueue(),
  myThreads(numThreads),
  running(false),
  myThrdCounter(0) {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing ThreadPool class...");
#endif
}

template <typename T, typename R>
ThreadPool<T, R>::~ThreadPool() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Destructing ThreadPool class...");
#endif
  stop();
}

template <typename T, typename R>
void ThreadPool<T, R>::start() {
  std::unique_lock<std::mutex> l(myMutex);
  if (!(running)) {
    for (auto& thrd: myThreads) {
      thrd = std::thread(&ThreadPool<T, R>::thread, this);
    }
    running = true;
  }
}

template <typename T, typename R>
void ThreadPool<T, R>::stop() {
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

template <typename T, typename R>
void ThreadPool<T, R>::append(const R& r) {
  size_t queueSize = 0;
  {
    std::unique_lock<std::mutex> l(myMutex);
    // TODO Add here a proper code block if a queue limit is necessary!
    myQueue.emplace(r);
    queueSize = myQueue.size();
    myCondVar.notify_one();
  }
#if defined(DEBUG) && VERBOSENESS == 1
  static unsigned long last = 0;
  auto t = std::chrono::system_clock::now().time_since_epoch();
  unsigned long msecEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
  long tdiff = msecEpoch - last;
  if (tdiff > 1000) {
    last = msecEpoch;
    std::cout << "Current queue size: " << queueSize << '\n';
  }
#endif
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Appending request to thread pool");
  Console::log("Current queue size: " + str(queueSize));
#endif
}

template <typename T, typename R>
void ThreadPool<T, R>::thread() {
  R r(UDP_BUFFER_SIZE);
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
          Console::log(getUTCTime() + " [DEBUG] Exiting thread n. " + str(thrdNum) + "...");
#endif
          break;
        }
      }
      r = myQueue.front();
      myQueue.pop();
      // TODO Add here a proper code block if a queue limit is necessary!
    }
#if defined(DEBUG) && VERBOSENESS > 2
    Console::log(getUTCTime() + " [DEBUG] Thread n." + str(thrdNum) + " is computing request " + str(r.referDatagram().token()) + "-" + str(r.referDatagram().tid()));
#endif
    myTask(r);  // Do the task, passing it the request informations...
  }
}

template class ThreadPool<TCPResponse, TCPRequestInfo>;
template class ThreadPool<UDPResponse, UDPRequestInfo>;
template class ThreadPool<UDPSender, UDPRequestInfo>;
