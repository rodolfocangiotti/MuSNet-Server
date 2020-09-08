#include <cassert>
#include <iostream>
#include "Console.h"
#include "prettyprint.h"



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

void Console::log(const char* s, const Color c) {
  std::string is(s);
  is = prettify(is, c);
  append(is);
}

void Console::log(const std::string& s, const Color c) {
  std::string is = static_cast<std::string>(s);
  is = prettify(is, c);
  append(is);
}

void Console::append(const std::string& s) {
  Locker l(mutex);
  assert(("Console thread is not running.", isRunning));
  queue.emplace(s);
  condVar.notify_one();
}

std::string& Console::prettify(std::string& s, const Color c) {
  if (c == Color::Black) {
    s.insert(0, BLACK);
    s.append(RESET);
  } else if (c == Color::Red) {
    s.insert(0, RED);
    s.append(RESET);
  } else if (c == Color::Green) {
    s.insert(0, GREEN);
    s.append(RESET);
  } else if (c == Color::Yellow) {
    s.insert(0, YELLOW);
    s.append(RESET);
  } else if (c == Color::Blue) {
    s.insert(0, BLUE);
    s.append(RESET);
  } else if (c == Color::Magenta) {
    s.insert(0, MAGENTA);
    s.append(RESET);
  } else if (c == Color::Cyan) {
    s.insert(0, CYAN);
    s.append(RESET);
  } else if (c == Color::White) {
    s.insert(0, WHITE);
    s.append(RESET);
  }
  return s;
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
