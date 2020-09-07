#include "Chrono.h"

void Chrono::setPoint() {
  t0 = SysClock::now();
}

double Chrono::timeDelta() {
  SysTime t1 = SysClock::now();
  double d = std::chrono::duration_cast<NanoSecs>(t1 - t0).count();
  return d;
}

Chrono::SysTime Chrono::t0 = SysClock::now();
