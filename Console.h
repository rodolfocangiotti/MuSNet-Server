#ifndef CONSOLE_H
#define CONSOLE_H

#include <queue>
#include <string>
#include <thread>

class Console {
public:
  static void start();
  static void stop();
  /*
  template <typename T>
  static void log(const T n);
  */
  static void log(const char* s);
  static void log(const std::string& s);
private:
  typedef std::queue<std::string> StringQueue;
  typedef std::unique_lock<std::mutex> Locker;
  static void runThread();
  static std::thread thread;
  static std::mutex mutex;
  static std::condition_variable condVar;
  static StringQueue queue;
  static bool isRunning;
};

#endif
