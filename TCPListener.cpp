#include <unistd.h>
#include <cassert>
#include <iostream>
#include "TCPListener.h"
#include "commons.h"
#include "prettyprint.h"
#include "utils.h"

TCPListenerException::TCPListenerException(const std::string e) noexcept:
  error(e) {
  error.insert(0, "[ERROR] TCPListenerException: ");
  // TODO
}

TCPListenerException::~TCPListenerException() {
  // TODO
}

const char* TCPListenerException::what() const noexcept {
  return error.c_str();
}

TCPListener::TCPListener(Manager& m):
  mySockFD(0),
  myAddrss(), clieAddrss(),
  myAddrssLen(0), clieAddrssLen(0),
  myManager(m),
  mySegment(TCP_BUFFER_SIZE),
  active(false),
  myMutex(),
  myThread() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Constructing TCPListener class..." << '\n';
#endif
}

TCPListener::~TCPListener() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Destructing TCPListener class..." << '\n';
#endif
}

void TCPListener::initSocket() {
#if defined(DEBUG) && VERBOSENESS > 1
  std::cout << getUTCTime() << " [DEBUG] Initializing TCPListener socket..." << '\n';
#endif
  if ((mySockFD = socket(PF_INET, SOCK_STREAM, 0)) < 0) { // Create socket file descriptor for TCP protocol...
    perror("socket()");
    throw TCPListenerException("Socket creation failed.");
  }
  int enable = 1;
  if (setsockopt(mySockFD, SOL_SOCKET, SO_REUSEADDR, (const char*) &enable, sizeof enable) < 0) { // TODO Check if SO_REUSEADDR is okay...
    perror("setsockopt()");
    throw TCPListenerException("Socket option setting failed.");
  }
}

void TCPListener::bindSocket(const PortNum pn) {
  myAddrss.sin_family = AF_INET;  // Fill address information...
  myAddrss.sin_addr.s_addr = INADDR_ANY;
  myAddrss.sin_port = htons(pn);
  myAddrssLen = sizeof myAddrss;
  if (bind(mySockFD, (const struct sockaddr*) &myAddrss, myAddrssLen) < 0) {  // Bind the socket with the client address...
    perror("bind()");
    throw TCPListenerException("Socket bind failed.");
  }
#if defined(DEBUG) && VERBOSENESS > 1
  if (getsockname(mySockFD, (struct sockaddr*) &myAddrss, &myAddrssLen) < 0) {
    perror("getsockname()");
  } else {
    std::cout << getUTCTime() << " [DEBUG] Socket bound on " << inet_ntoa(myAddrss.sin_addr) << ":" << ntohs(myAddrss.sin_port) << "..." << '\n'; // TODO Convert to warning or info?
  }
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
    throw TCPListenerException("Listen start failed."); // TODO Define exception message...
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
    int descrAmount = select(currMaxFD + 1, &currSet, NULL, NULL, &timeout);
    if (!(descrAmount > 0)) {
      if (descrAmount < 0) {
        perror("select()");
        std::cerr << getUTCTime() << RED << " [ERROR] Error receiving request/segment!" << RESET << '\n';
      } else {  // descrAmount is equal to 0...
#if defined(DEBUG) && VERBOSENESS > 2
        std::cout << getUTCTime() << " [DEBUG] TCP timeout reached!" << '\n';
#endif
      }
      currSet = nextSet;
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
#if defined(DEBUG) && VERBOSENESS > 0
          std::cout << getUTCTime() << " [DEBUG] New TCP connection accepted!" << '\n';
#endif
          FD_SET(newSockFD, &nextSet);
          if (newSockFD > currMaxFD) {
            nextMaxFD = newSockFD;
          }
        } else {
          // ***** RECEIVE BLOCK *****
          int bytes = receive(i, mySegment.pointWritableBuffer(), TCP_BUFFER_SIZE);
          if (bytes < 0 ) {
            perror("recv()");
          } else if (bytes == 0) {  // Client is disconnected...
            shutdown(SHUT_RDWR, i);
            close(i);
#if defined(DEBUG) && VERBOSENESS > 0
            std::cout << getUTCTime() << " [DEBUG] TCP connection closed!" << '\n';
#endif
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
            if (mySegment.header() == ENTRY_REQUEST) {
              ClientToken t = myManager.addClient();
              if (t < 0) {
                std::cerr << getUTCTime() << RED << " [ERROR] Impossible to add client!" << RESET << '\n';
                continue;
                // TODO Add error response!
              }
              mySegment.buildEntryResponse(t);
            } else if (mySegment.header() == EXIT_REQUEST) {
              ClientToken t = mySegment.token();
              int res = myManager.removeClient(t);
              if (res < 0) {
                std::cerr << getUTCTime() << RED << " [ERROR] Impossible to remove client!" << RESET << '\n';
                continue;
              }
              mySegment.buildExitResponse();
            } else {
              std::cerr << getUTCTime() << RED << " [ERROR] Not consistent header of TCP segment!" << RESET << '\n';
              continue;
              // TODO Add error response!
            }
            int bytes = send(i, mySegment.rawBuffer(), mySegment.size());
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

int TCPListener::send(const SocketFD sfd, const void* buff, const size_t s) {
  return ::send(sfd, buff, s, 0);
}

int TCPListener::receive(const SocketFD sfd, void* buff, const size_t s) {
  return recv(sfd, buff, s, 0);
}

bool TCPListener::listening() {
  std::lock_guard<std::mutex> l(myMutex);
  return active;
}
