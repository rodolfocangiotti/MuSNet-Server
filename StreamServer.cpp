#include <chrono>
#include <exception>
#include <iostream>
#include "StreamServer.h"
#include "prettyprint.h"

StreamServer::StreamServer():
  active(false),
  myManager(),
  myTCPListnr(myManager),
  myUDPListnr(myUDPThrds),
  myUDPResp(myManager),
  myUDPThrds(myUDPResp) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing StreamServer class..." << '\n';
#endif
}

StreamServer::~StreamServer() {
  stop();
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing StreamServer class..." << '\n';
#endif
}

void StreamServer::configure(PortNum tcpPN, PortNum udpPN) {
  while (true) {
    try {
      myTCPListnr.configure(tcpPN);
      break;
    } catch (TCPListenerException& e) {
      std::cerr << RED << e.what() << RESET << '\n';
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  }
  while (true) {
    try {
      myUDPListnr.configure(udpPN);
      break;
    } catch (UDPListenerException& e) {
      std::cerr << RED << e.what() << RESET << '\n';
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  }
}

void StreamServer::start() {
  if (!(active)) {
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Starting TCP listener..." << '\n';
#endif
    myTCPListnr.start();
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Starting UDP listener..." << '\n';
#endif
    myUDPListnr.start();
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Starting thread pool..." << '\n';
#endif
    myUDPThrds.start();
    active = true;
  }
}

void StreamServer::stop() {
  if (active) {
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Stopping TCP listener..." << '\n';
#endif
    myTCPListnr.stop();
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Stopping UDP listener..." << '\n';
#endif
    myUDPListnr.stop();
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << "[DEBUG] Stopping thread pool..." << '\n';
#endif
    myUDPThrds.stop();
    active = false;
  }
}
