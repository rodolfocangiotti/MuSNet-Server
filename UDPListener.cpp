#include <unistd.h>
#include <cassert>
#include <iostream>
#include "UDPListener.h"
#include "commons.h"
#include "prettyprint.h"

UDPListener::UDPListener(ThreadPool<UDPResponse>& tp):
  mySockFD(0),
  myAddrss(), clieAddrss(),
  myAddrssLen(0), clieAddrssLen(0),
  myTimeout(),
  myRequestInfo(UDP_BUFFER_SIZE),
  myThreadPool(tp),
  myMutex(),
  myThread(),
  active(false) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing UDPListener class..." << std::endl;
#endif
}

UDPListener::~UDPListener() {
  stop(); // Make sure the listening is stopped...
  close(mySockFD);
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing UDPListener class..." << std::endl;
#endif
}

void UDPListener::initSocket() {
#if defined(DEBUG) && VERBOSENESS > 1
  std::cout << "[DEBUG] Initializing UDPListener socket..." << std::endl;
#endif
  if ((mySockFD = socket(PF_INET, SOCK_DGRAM, 0)) < 0) { // Create socket file descriptor for UDP protocol...
    perror("socket()");
    exit(EXIT_FAILURE);
    //throw UDPListenerException("Socket creation failed.");
  }
  int enable = 1;
  if (setsockopt(mySockFD, SOL_SOCKET, SO_REUSEADDR, (const char*) &enable, sizeof enable) < 0) { // TODO Check if SO_REUSEADDR is okay...
    perror("setsockopt()");
    exit(EXIT_FAILURE);
    //throw UDPListenerException("Socket options setting failed.");
  }
}

void UDPListener::bindSocket(PortNum pn) {
  myAddrss.sin_family = AF_INET;  // Fill UDPListener information...
  myAddrss.sin_addr.s_addr = INADDR_ANY;
  myAddrss.sin_port = htons(pn);
  myAddrssLen = sizeof myAddrss;
  if (bind(mySockFD, (const struct sockaddr*) &myAddrss, myAddrssLen) < 0) {  // Bind the socket with the client address...
    perror("bind()");
    exit(EXIT_FAILURE);
    //throw UDPListenerException("Socket bind failed.");
  }
#if defined(DEBUG) && VERBOSENESS > 1
  if (getsockname(mySockFD, (struct sockaddr*) &myAddrss, &myAddrssLen) < 0) {
    perror("getsockname()");
    exit(EXIT_FAILURE);
  }
  std::cout << "[DEBUG] Socket bound on " << inet_ntoa(myAddrss.sin_addr) << ":" << ntohs(myAddrss.sin_port) << "..." << std::endl; // TODO Convert to warning or info?
#endif
}

void UDPListener::initClientAddress() {
  clieAddrssLen = sizeof clieAddrss;
}

void UDPListener::configure(PortNum pn) {
  initSocket();
  bindSocket(pn);
  initClientAddress();
}

void UDPListener::listen() {
  // Define a set of file descriptors...
  fd_set currSet;
  FD_ZERO(&currSet);
  FD_SET(mySockFD, &currSet);
  fd_set nextSet = currSet;
  myTimeout = {1, 0};

  while (listening()) {
    // Receive datagram from client...
    UDPDatagram& reqstDatagram = myRequestInfo.referWritableDatagram();
    int res = receiveWithTimeout(&currSet, reqstDatagram.pointWritableBuffer(), UDP_BUFFER_SIZE);
    if (!(res > 0)) {
      if (res < 0) {
        perror("receiveWithTimeout()");
        std::cerr << RED << "[ERROR] Error receiving datagram!" << RESET << std::endl;
      } else {  // res is equal to 0...
#if defined(DEBUG) && VERBOSENESS > 2
        std::cout << "[DEBUG] UDP timeout reached!" << std::endl; // TODO Take it back to debug output...
#endif
      }
      currSet = nextSet;
      continue;
    }

#if defined(DEBUG) && VERBOSENESS > 2
    std::cout << "[DEBUG] Datagram received!" << std::endl;
#endif
    myRequestInfo.setFileDescriptor(mySockFD);
    myRequestInfo.setAddress(&clieAddrss, &clieAddrssLen);
    myThreadPool.append(myRequestInfo);
    currSet = nextSet;
  }
}

void UDPListener::start() {
  std::lock_guard<std::mutex> l(myMutex);
  if (!(active)) {
    myThread = std::thread(&UDPListener::listen, this);
    active = true;
  }
}

void UDPListener::stop() {
  bool join = false;
  {
    std::lock_guard<std::mutex> l(myMutex);
    if (active) {
      active = false;
      join = true;
    }
  }
  if (join) {
    if (myThread.joinable()) {
      myThread.join();
    }
  }
}

int UDPListener::receive(void* buff, size_t s) {
  return recvfrom(mySockFD, (char*) buff, s, 0, (struct sockaddr*) &clieAddrss, &clieAddrssLen);
}

int UDPListener::receiveWithTimeout(fd_set* fds, void* buff, size_t s) {
  int descrAmount = select(mySockFD + 1, fds, NULL, NULL, &myTimeout);
  if (descrAmount > 0) {
    assert(FD_ISSET(mySockFD, fds));
    return receive(buff, s);
  }
  return descrAmount;
}

bool UDPListener::listening() {
  std::lock_guard<std::mutex> l(myMutex);
  return active;
}
