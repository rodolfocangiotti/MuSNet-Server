#ifndef UDP_RESPONSE_H
#define UDP_RESPONSE_H

#include <mutex>
#include "Manager.h"
#include "RequestInfo.h"
#include "UDPDatagram.h"

class UDPResponse {
public:
  UDPResponse(Manager& m);
  ~UDPResponse();
  void operator()(const RequestInfo& r); // TODO Check if copy of object is better than reference...
private:
  void send(const void* buff, size_t s);
  std::mutex myMutex;
  Manager& myManager;
  UDPDatagram myPayload;
};

#endif
