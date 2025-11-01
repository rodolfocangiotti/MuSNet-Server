#ifndef UDP_RESPONSE_H
#define UDP_RESPONSE_H

#include "Manager.h"
#include "ThreadPool.h"
#include "UDPDatagram.h"
#include "UDPRequestInfo.h"
#include "UDPSender.h"

class UDPResponse {
public:
  UDPResponse(Manager& m, ThreadPool<UDPSender, UDPRequestInfo>& tp);
  ~UDPResponse();
  // ********************
  void operator()(UDPRequestInfo& r);
private:
  Manager& myManager;
  ThreadPool<UDPSender, UDPRequestInfo>& myUDPSendr;
};

#endif
