#include <unistd.h>
#include <cassert>
#include <iostream>
#include "UDPListener.h"
#include "commons.h"

UDPListener::UDPListener(ThreadPool<UDPResponse>& tp):
  mySockFD(0),
  myAddrss(), clieAddrss(),
  myAddrssLen(0), clieAddrssLen(0),
  myRequestInfo(UDP_BUFFER_SIZE),
  myThreadPool(tp),
  myMutex(),
  myThread(),
  active(false) {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing UDPListener class..." << std::endl;
#endif
}

UDPListener::~UDPListener() {
  stop(); // Make sure the listening is stopped...
  close(mySockFD);
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing UDPListener class..." << std::endl;
#endif
}

void UDPListener::initSocket() {
#ifdef DEBUG
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
#ifdef DEBUG
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
  while (listening()) {
    // Receive datagram from client...
    if (receiveWithTimeout(myRequestInfo.pointWritableBuffer(), UDP_BUFFER_SIZE) < 0) {
#ifdef DEBUG
      /*
      std::cout << "[DEBUG] Error receiving datagram or timeout reached!" << std::endl;
      */
#endif
      continue;
    }
#ifdef DEBUG
    /*
    std::cout << "[DEBUG] Datagram received!" << std::endl;
    */
#endif
    myRequestInfo.setFileDescriptor(mySockFD);
    myRequestInfo.setAddress(&clieAddrss, &clieAddrssLen);
    myThreadPool.append(myRequestInfo);
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
  return recvfrom(mySockFD, buff, s, 0, (struct sockaddr*) &clieAddrss, &clieAddrssLen);
}

int UDPListener::receiveWithTimeout(void* buff, size_t s) {
  struct timeval timeout = {1, 0}; // Define a timeout of 1 second...
  fd_set descrSet;
  FD_ZERO(&descrSet);
  FD_SET(mySockFD, &descrSet);
  int descrAmount = select(mySockFD + 1, &descrSet, NULL, NULL, &timeout);
  if (descrAmount > 0) {
    assert(FD_ISSET(mySockFD, &descrSet));
    return receive(buff, s);
  }
  return -1;  // Return undefined error...
}

bool UDPListener::listening() {
  std::lock_guard<std::mutex> l(myMutex);
  return active;
}
