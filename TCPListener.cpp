#include <unistd.h>
#include <cassert>
#include <iostream>
#include "TCPListener.h"
#include "commons.h"

TCPListener::TCPListener(Manager& m):
  mySockFD(0),
  myAddrss(), clieAddrss(),
  myAddrssLen(), clieAddrssLen(),
  myPayload(TCP_BUFFER_SIZE),
  myManager(m),
  active(false),
  myMutex(),
  myThread() {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing TCPListener class..." << std::endl;
#endif
}

TCPListener::~TCPListener() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing TCPListener class..." << std::endl;
#endif
}

void TCPListener::initSocket() {
#ifdef DEBUG
  std::cout << "[DEBUG] Initializing TCPListener socket..." << std::endl;
#endif
  if ((mySockFD = socket(PF_INET, SOCK_STREAM, 0)) < 0) { // Create socket file descriptor for TCP protocol...
    perror("socket()");
    exit(EXIT_FAILURE);
    //throw TCPListenerException("Socket creation failed.");
  }
  int enable = 1;
  if (setsockopt(mySockFD, SOL_SOCKET, SO_REUSEADDR, (const char*) &enable, sizeof enable) < 0) { // TODO Check if SO_REUSEADDR is okay...
    perror("setsockopt()");
    exit(EXIT_FAILURE);
    //throw TCPListenerException("Socket options setting failed.");
  }
}

void TCPListener::bindSocket(const PortNum pn) {
  myAddrss.sin_family = AF_INET;  // Fill address information...
  //myAddrss.sin_addr.s_addr = INADDR_ANY;
  myAddrss.sin_addr.s_addr = htonl(INADDR_ANY);
  myAddrss.sin_port = htons(pn);
  myAddrssLen = sizeof myAddrss;
  if (bind(mySockFD, (const struct sockaddr*) &myAddrss, myAddrssLen) < 0) {  // Bind the socket with the client address...
    perror("bind()");
    exit(EXIT_FAILURE);
    //throw TCPListenerException("Socket bind failed.");
  }
#ifdef DEBUG
  if (getsockname(mySockFD, (struct sockaddr*) &myAddrss, &myAddrssLen) < 0) {
    perror("getsockname()");
    exit(EXIT_FAILURE);
  }
  std::cout << "[DEBUG] Socket bound on " << inet_ntoa(myAddrss.sin_addr) << ":" << ntohs(myAddrss.sin_port) << "..." << std::endl; // TODO Convert to warning or info?
#endif
}

void TCPListener::initClientAddress() {
  clieAddrssLen = sizeof clieAddrss;
}

void TCPListener::configure(const PortNum pn) {
  initSocket();
  bindSocket(pn);
  initClientAddress();
}

void TCPListener::listen() {
  if (::listen(mySockFD, 5) < 0) {  // TODO Check backlog parameter...
    perror("listen()");
    //throw TCPListenerException("Listen start failed."); // TODO Define exception message...
  }
  fd_set currSet;
  FD_ZERO(&currSet);
  FD_SET(mySockFD, &currSet);
  fd_set nextSet = currSet;
  SocketFD currMaxFD = mySockFD;
  SocketFD nextMaxFD = mySockFD;
  struct timeval timeout = {1, 0}; // Define a timeout of 1 second...
  while (listening()) {
    // Receive segment from client...
#ifdef DEBUG
    std::cout << ">>> Current max. file descr.: " << currMaxFD << std::endl;  // TODO Remove it!
#endif
    int descrAmount = select(currMaxFD + 1, &currSet, NULL, NULL, &timeout);
    if (!(descrAmount > 0)) {
      if (descrAmount < 0) {
        perror("select()");
      }
#ifdef DEBUG
      std::cout << "[DEBUG] Error receiving segment or timeout reached!" << std::endl;
#endif
      currSet = nextSet;  // WTF?
      continue;
    }
    // ***** SOCKET ITERATION BLOCK ******
    for(int i = 0; i < currMaxFD + 1; i++) {
      if (FD_ISSET(i, &currSet)) {
        if (i == mySockFD) {
          SocketFD newSockFD = accept(mySockFD, (struct sockaddr*) &clieAddrss, &clieAddrssLen);
          if (newSockFD < 0) {
            perror("accept()");
            continue;
          }
#ifdef DEBUG
          std::cout << "New TCP connection accepted!" << std::endl;
#endif
          FD_SET(newSockFD, &nextSet);
          if (newSockFD > currMaxFD) {
            nextMaxFD = newSockFD;
          }
        } else {
          // ***** RECEIVE BLOCK *****
          int bytes = receive(i, myPayload.pointWritableBuffer(), TCP_BUFFER_SIZE);
          if (bytes < 0 ) {
            perror("recv()");
          } else if (bytes == 0) {  // Client is disconnected...
            shutdown(SHUT_RDWR, i);
            close(i);
            std::cout << "TCP connection closed!" << std::endl;
            FD_CLR(i, &nextSet);
            if (i == currMaxFD) {
              for (int j = 0; j < currMaxFD; j++) { // Update maximum file descriptor value...
                if (FD_ISSET(j, &nextSet)) {
                  nextMaxFD = j;
                }
              }
            }
          } else {
            // Manage received message
            if (myPayload.header() == ENTRY_REQUEST) {
              StreamClient::Token t = myManager.addClient();
              myPayload.buildEntryResponse(t);
            } else if (myPayload.header() == EXIT_REQUEST) {
              StreamClient::Token t = myPayload.token();
              myManager.removeClient(t);
              myPayload.buildExitResponse();
            } else {
              std::cerr << "[ERROR] Not consistent header of TCP segment!" << std::endl;
              continue;
            }
            int bytes = send(i, myPayload.rawBuffer(), myPayload.size());
            if (bytes < 0) {
              perror("send");
            }
          }
          // ***** END OF RECEIVE BLOCK *****
        }
      }
    }
    // ***** END OF SOCKET ITERATION BLOCK *****
    currSet = nextSet;
    currMaxFD = nextMaxFD; // Update maximum file descriptor value for the next cycle...
  }
}


void TCPListener::start() {
  std::lock_guard<std::mutex> l(myMutex);
  if (!(active)) {
    myThread = std::thread(&TCPListener::listen, this);
    active = true;
  }
}

void TCPListener::stop() {
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

int TCPListener::send(SocketFD sfd, const void* buff, size_t s) {
  return ::send(sfd, buff, s, 0);
}

int TCPListener::receive(SocketFD sfd, void* buff, size_t s) {
  return recv(sfd, buff, s, 0);
}

bool TCPListener::listening() {
  std::lock_guard<std::mutex> l(myMutex);
  return active;
}
