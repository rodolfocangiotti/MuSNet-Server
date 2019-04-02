#ifndef UDP_RESPONSE_H
#define UDP_RESPONSE_H

#include "Manager.h"
#include "RequestInfo.h"
#include "UDPDatagram.h"

class UDPResponse {
public:
  UDPResponse(Manager& m);
  ~UDPResponse();
  void operator()(const RequestInfo& r);
private:
  void send(const void* buff, size_t s);
  Manager& myManager;
  UDPDatagram myDatagram;
};

#endif
