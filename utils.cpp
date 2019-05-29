#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "utils.h"

std::string getUTCTime() {
  auto t = std::chrono::system_clock::now().time_since_epoch();
  unsigned long msecEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(t).count();
  unsigned long msecPerDay = msecEpoch % (24 * 60 * 60 * 1000);
  unsigned long msecVal = msecPerDay % 1000;
  unsigned long secVal = (msecPerDay / 1000) % 60;
  unsigned long minVal = (msecPerDay / (60 * 1000)) % 60;
  unsigned long hourVal = msecPerDay / (60 * 60 * 1000);

  std::stringstream ss;
  ss << std::setfill('0') << std::setw(2) << hourVal << ":";
  ss << std::setfill('0') << std::setw(2) << minVal << ":";
  ss << std::setfill('0') << std::setw(2) << secVal << ".";
  ss << std::setfill('0') << std::setw(3) << msecVal << " UTC";
  return ss.str();
}
