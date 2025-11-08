#include <chrono>
#include <iostream>
#include "Console.h"
#include "StreamServer.h"
#include "prettyprint.h"
#include "utils.h"

StreamServer::StreamServer():
  active(false),
  myManager(),
  myTCPListnr(myManager, _tcp_response_threads),
  _tcp_response(myManager),
  _tcp_response_threads(_tcp_response),
  myUDPListnr(myUDPThrds),
  myUDPResp(myManager, myUDPOthrThrds),
  myUDPSendr(),
  myUDPThrds(myUDPResp),
  myUDPOthrThrds(myUDPSendr) {  // Sender is one thread only. TODO To test!
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing StreamServer class...");
#endif
}

StreamServer::~StreamServer() {
  stop();
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing StreamServer class...");
#endif
}

void StreamServer::configure(PortNum tcpPN, PortNum udpPN) {
  for (uint16_t i {0}; i < 5; i++) {
    try {
      myTCPListnr.configure(tcpPN);
      break;
    } catch (TCPListenerException& e) {
      std::cerr << RED << e.what() << RESET << '\n';
      std::this_thread::sleep_for(std::chrono::seconds(10));
    }
  }
  for (uint16_t i {0}; i < 5; i++) {
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
    Console::log(getUTCTime() + " [DEBUG] Starting TCP listener...");
#endif
    myTCPListnr.start();
#if defined(DEBUG) && VERBOSENESS > 1
    Console::log(getUTCTime() + " [DEBUG] Starting UDP listener...");
#endif
    myUDPListnr.start();
#if defined(DEBUG) && VERBOSENESS > 1
    Console::log(getUTCTime() + " [DEBUG] Starting thread pool...");
#endif
    _tcp_response_threads.start();
    myUDPThrds.start();
    myUDPOthrThrds.start();
    active = true;
  }
}

void StreamServer::stop() {
  if (active) {
#if defined(DEBUG) && VERBOSENESS > 1
    Console::log(getUTCTime() + " [DEBUG] Stopping TCP listener...");
#endif
    myTCPListnr.stop();
#if defined(DEBUG) && VERBOSENESS > 1
    Console::log(getUTCTime() + " [DEBUG] Stopping UDP listener...");
#endif
    myUDPListnr.stop();
#if defined(DEBUG) && VERBOSENESS > 1
    Console::log(getUTCTime() + " [DEBUG] Stopping thread pool...");
#endif
    _tcp_response_threads.stop();
    myUDPThrds.stop();
    myUDPOthrThrds.stop();
    active = false;
  }
}
