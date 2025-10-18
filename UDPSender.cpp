#include <cmath>
#include <iostream>
#include "Chrono.h"
#include "Console.h"
#include "Profiler.h"
#include "UDPSender.h"

double average(double x) {
  /*
  static double sum = 0.0;
  static double cntr = 0.0;
  sum += x;
  cntr += 1.0;
  return sum / cntr;
  */
  static std::vector<double> mem(16384, 0.0);
  static int i = 0;
  i = i < mem.size() ? i : 0;
  mem[i] = x;
  i++;
  double sum = 0.0;
  for (int j = 0; j < mem.size(); j++) {
    sum += mem[j];
  }
  return sum / static_cast<double>(mem.size());
}

double standard_deviation(double x) {
  static std::vector<double> v(16384, 0.0);
  // Calculate average...
  static uint c = 0;  // Counter...
  static uint wi = 0; // Writing index...

  v[wi++] = x;
  wi = wi < v.size() ? wi : wi - v.size();

  double sum = 0.0;
  for (uint ri = 0; ri < v.size(); ri++) {
    sum += v[ri];
  }
  c++;
  uint sz = c < v.size() ? c : v.size();
  double avg = sum / sz;
  // Now deviation...
  double devSum = 0.0;
  for (uint ri = 0; ri < sz; ri++) {
    double val = v[ri] - avg;
    devSum += (val * val);  // A.k.a. val^2...
  }
  return sqrt(devSum / sz);
}


UDPSender::UDPSender() {
#if defined(DEBUG) && VERBOSENESS >= 3
  std::cout << "Constructing UDPSender..." << '\n';
#endif
}

UDPSender::~UDPSender() {
#if defined(DEBUG) && VERBOSENESS >= 3
  std::cout << "Destructing UDPSender..." << '\n';
#endif
}

void UDPSender::operator()(RequestInfo& r) {
  const UDPDatagram& reqstDatagram = r.referDatagram();
  Profiler::add_record(reqstDatagram.token(), reqstDatagram.tid(), Profiler::OperationID::RESPONSE_START);
  struct sockaddr_in addrss = r.address();
  socklen_t addrssLen = r.addressLength();
  SocketFD sockFD = r.fileDescriptor();
  // Time e = r.receiptTime(); TODO

// 29/05/2021
// #if defined(DEBUG) && VERBOSENESS > 1
//   double d = Chrono::timeDelta();
//   Console::log("Computation time: " + str(d * 1e-6) + " msecs. (average: " + str(average(d) * 1e-6) + ")");
// #endif
  int bytes = sendto(sockFD, (const char*) reqstDatagram.rawBuffer(), UDP_BUFFER_SIZE, 0, (const struct sockaddr*) &addrss, addrssLen);
  if (bytes < 0) { // TODO Test it!
    perror("sendto()");
  }
  Profiler::add_record(reqstDatagram.token(), reqstDatagram.tid(), Profiler::OperationID::UDP_DISPATCH);
  // 29/05/2021
  // RESTORE OLD DURATION TIME MONITORING...
  //Time now = std::chrono::high_resolution_clock::now();
  //Time rtime = r.receiptTime();
  //double d = std::chrono::duration_cast<std::chrono::nanoseconds>(now - rtime).count();
  //double avg = average(d);
  //double stddev = standard_deviation(d);
  //static uint64_t counter = 0;
  //if (!(counter % 256)) {
    //std::cout << "Comp. time: " << d * 1e-6 << " msecs. (avg: " << avg * 1e-6 << " - stddev: " << stddev * 1e-6 << ")\n";
  //}
  //counter++;
  //std::this_thread::sleep_for(std::chrono::microseconds(UDP_SEND_WAIT)); // Sleep time to reach 20Mbit/sec bandwidth...
  // VIEW ORIGINAL CODE ON LOCAL BRANCH...
}
