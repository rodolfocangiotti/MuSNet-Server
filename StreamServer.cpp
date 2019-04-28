#include <iostream>
#include "StreamServer.h"

StreamServer::StreamServer():
  myManager(),
  myUDPResp(myManager),
  myUDPThrds(myUDPResp),
  myTCPListnr(myManager),
  myUDPListnr(myUDPThrds) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing StreamServer class..." << std::endl;
#endif
}

StreamServer::~StreamServer() {
  stop();
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing StreamServer class..." << std::endl;
#endif
}

void StreamServer::configure(PortNum tcpPN, PortNum udpPN) {
  myTCPListnr.configure(tcpPN);
  myUDPListnr.configure(udpPN);
}

void StreamServer::start() {
  if (!(active)) {
    myTCPListnr.start();
    myUDPListnr.start();
    myUDPThrds.start();
    active = true;
  }
}

void StreamServer::stop() {
  if (active) {
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Stopping TCP listener..." << std::endl;
#endif
    myTCPListnr.stop();
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Stopping UDP listener..." << std::endl;
#endif
    myUDPListnr.stop();
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Stopping thread pool..." << std::endl;
#endif
    myUDPThrds.stop();
    active = false;
  }
}
