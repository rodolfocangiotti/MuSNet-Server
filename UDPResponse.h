#ifndef UDP_RESPONSE_H
#define UDP_RESPONSE_H

#include "Manager.h"
#include "RequestInfo.h"
#include "ThreadPool.h"
#include "UDPDatagram.h"
#include "UDPSender.h"

class UDPResponse {
public:
  UDPResponse(Manager& m, ThreadPool<UDPSender>& tp);
  ~UDPResponse();
  // ********************
  void operator()(RequestInfo& r);
private:
  Manager& myManager;
  ThreadPool<UDPSender>& myUDPSendr;
};

#endif
