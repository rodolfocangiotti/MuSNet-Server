#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include "Manager.h"
#include "TCPListener.h"
#include "ThreadPool.h"
#include "UDPListener.h"
#include "UDPResponse.h"
#include "types.h"

class StreamServer {
public:
  StreamServer();
  ~StreamServer();
  void configure(PortNum tcpPN, PortNum udpPN); // Set TCP and UDP port number for listening...
  void start();
  void stop();
private:
  bool active;
  Manager myManager;
  UDPResponse myUDPResp;
  ThreadPool<UDPResponse> myUDPThrds;
  TCPListener myTCPListnr;
  UDPListener myUDPListnr;
};

#endif
