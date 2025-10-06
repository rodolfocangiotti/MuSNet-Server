#ifndef UDP_SENDER_H
#define UDP_SENDER_H

#include "RequestInfo.h"

class UDPSender {
public:
  UDPSender();
  ~UDPSender();
  void operator()(RequestInfo& r);
};

#endif
