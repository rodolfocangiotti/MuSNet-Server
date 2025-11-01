#ifndef STREAM_SERVER_H
#define STREAM_SERVER_H

#include "Manager.h"
#include "TCPListener.h"
#include "TCPRequestInfo.h"
#include "TCPResponse.h"
#include "ThreadPool.h"
#include "UDPListener.h"
#include "UDPRequestInfo.h"
#include "UDPResponse.h"
#include "UDPSender.h"
#include "types.h"

class StreamServer {
public:
  StreamServer();
  ~StreamServer();
  // ********************
  void configure(PortNum tcpPN, PortNum udpPN); // Set TCP and UDP port number for listening...
  void start();
  void stop();
private:
  bool active;
  Manager myManager;
  TCPListener myTCPListnr;
  TCPResponse _tcp_response;
  ThreadPool<TCPResponse, TCPRequestInfo> _tcp_response_threads;
  UDPListener myUDPListnr;
  UDPResponse myUDPResp;
  UDPSender myUDPSendr;
  ThreadPool<UDPResponse, UDPRequestInfo> myUDPThrds;
  ThreadPool<UDPSender, UDPRequestInfo> myUDPOthrThrds;
};

#endif
