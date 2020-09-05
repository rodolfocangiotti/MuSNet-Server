#include <cassert>
#include <iostream>
#include "Console.h"



void Console::start() {
  Locker l(mutex);
  if (!(isRunning)) {
    thread = std::thread(&Console::runThread);
    isRunning = true;
  }
}

void Console::stop() {
  {
    Locker l(mutex);
    if (isRunning) {
      isRunning = false;
      condVar.notify_all();
    }
  }
  assert(("Console thread is not joinable.", thread.joinable()));
  thread.join();
}

/*
template <typename T>
void Console::log(const T n) {
  Locker l(mutex);
  assert(("Console thread is not running.", isRunning));
  queue.emplace(std::to_string(n));
  condVar.notify_one();
}
*/

void Console::log(const char* s) {
  Locker l(mutex);
  assert(("Console thread is not running.", isRunning));
  queue.emplace(s);
  condVar.notify_one();
}

void Console::log(const std::string& s) {
  Locker l(mutex);
  assert(("Console thread is not running.", isRunning));
  queue.emplace(s);
  condVar.notify_one();
}

void Console::runThread() {
  std::string s;
  while (true) {
    {
      Locker l(mutex);
      if (queue.empty()) {
        if (isRunning) {
          condVar.wait(l);
          continue;
        } else {
          break;  // Exit loop only if termination has been notified and queue is empty...
        }
      }
      s = queue.front();
      queue.pop();
    }
    std::cout << s << '\n';
  }
}

std::thread Console::thread;
std::mutex Console::mutex;
std::condition_variable Console::condVar;
Console::StringQueue Console::queue;
bool Console::isRunning = false;

/*
template void Console::log(const short);
template void Console::log(const unsigned short);
template void Console::log(const int);
template void Console::log(const unsigned int);
template void Console::log(const long);
template void Console::log(const unsigned long);
*/
