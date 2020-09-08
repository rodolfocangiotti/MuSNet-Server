#ifndef CONSOLE_H
#define CONSOLE_H

#include <queue>
#include <string>
#include <thread>

typedef enum {
  Default,
  Black,
  Red,
  Green,
  Yellow,
  Blue,
  Magenta,
  Cyan,
  White
} Color;

class Console {
public:
  static void start();
  static void stop();
  /*
  template <typename T>
  static void log(const T n);
  */
  static void log(const char* s, const Color c = Color::Default);
  static void log(const std::string& s, const Color c = Color::Default);
private:
  typedef std::queue<std::string> StringQueue;
  typedef std::unique_lock<std::mutex> Locker;
  static void append(const std::string& s);
  static std::string& prettify(std::string& s, const Color c);
  static void runThread();
  static std::thread thread;
  static std::mutex mutex;
  static std::condition_variable condVar;
  static StringQueue queue;
  static bool isRunning;
};

#endif
