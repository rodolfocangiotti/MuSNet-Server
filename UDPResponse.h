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
  // ********************
  void operator()(RequestInfo& r);
private:
  Manager& myManager;
  std::mutex myMutex;
};

#endif
