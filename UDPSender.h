#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include "UDPRequestInfo.h"

class UDPSender {
public:
  UDPSender();
  ~UDPSender();
  void operator()(UDPRequestInfo& r);
};

#endif
