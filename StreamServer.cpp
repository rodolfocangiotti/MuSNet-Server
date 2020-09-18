#include <chrono>
#include <iostream>
#include "Console.h"
#include "StreamServer.h"
#include "prettyprint.h"
#include "utils.h"

StreamServer::StreamServer():
  active(false),
  myManager(),
  myTCPListnr(myManager),
  myUDPListnr(myUDPThrds),
  myUDPResp(myManager),
  myUDPThrds(myUDPResp) {
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
    myUDPThrds.start();
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
    myUDPThrds.stop();
    active = false;
  }
}
