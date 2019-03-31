#ifndef UDP_RESPONSE_H
#define UDP_RESPONSE_H

#include "RequestInfo.h"
#include "Manager.h"

class UDPResponse {
public:
  UDPResponse(Manager& m);
  ~UDPResponse();
  void operator()(const RequestInfo& r);
private:
  Manager& myManager;
};

#endif
