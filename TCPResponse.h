#ifndef TCP_RESPONSE_H
#define TCP_RESPONSE_H

#include "Manager.h"
#include "TCPRequestInfo.h"
#include "ThreadPool.h"

class TCPResponse {
public:
  TCPResponse(Manager& manager);
  ~TCPResponse();
  // ********************
  void operator()(TCPRequestInfo& request);
private:
  Manager& _manager;
};

#endif
