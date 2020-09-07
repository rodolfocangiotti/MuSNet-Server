#ifndef CHRONO_H
#define CHRONO_H

#include <chrono>

class Chrono {
public:
  static void setPoint();
  static double timeDelta();
private:
  typedef std::chrono::nanoseconds NanoSecs;
  typedef std::chrono::system_clock SysClock;
  typedef std::chrono::time_point<SysClock> SysTime;
  static SysTime t0;
};

#endif
