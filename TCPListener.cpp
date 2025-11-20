#include <unistd.h>
#include <cassert>
#include <iostream>
#include <string>
#include "Console.h"
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

TCPListener::TCPListener(Manager& m, ThreadPool<TCPResponse, TCPRequestInfo>& tp):
  mySockFD(0),
  myAddrss(), clieAddrss(),
  myAddrssLen(0), clieAddrssLen(0),
  myManager(m),
  active(false),
  myMutex(),
  myThread(),
  myThreadPool(tp),
  myRequestInfo(UDP_BUFFER_SIZE) {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing TCPListener class...");
#endif
}

TCPListener::~TCPListener() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Destructing TCPListener class...");
#endif
}

void TCPListener::initSocket() {
#if defined(DEBUG) && VERBOSENESS > 1
  Console::log(getUTCTime() + " [DEBUG] Initializing TCPListener socket...");
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
    Console::log(getUTCTime() + " [DEBUG] Socket bound on " + inet_ntoa(myAddrss.sin_addr) + ":" + str(ntohs(myAddrss.sin_port)) + "..."); // TODO Convert to warning or info?
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
  struct timeval T = timeout;

  TCPSegment& request_segment = myRequestInfo.refer_writable_segment();
  while (listening()) {
    // Receive segment from client...
    int descrAmount = select(currMaxFD + 1, &currSet, NULL, NULL, &timeout);
    if (descrAmount <= 0) {
      if (descrAmount < 0) {
        perror("select()");
        std::cerr << getUTCTime() << RED << " [ERROR] Error receiving request/segment!" << RESET << '\n';
      } else {  // descrAmount is equal to 0...
#if defined(DEBUG) && VERBOSENESS > 2
        Console::log(getUTCTime() + " [DEBUG] TCP timeout reached!");
#endif
      }
      currSet = nextSet;
      currMaxFD = nextMaxFD;
      timeout = T;
      continue;
    }
    // ***** SOCKET ITERATION BLOCK ******
    for(int i = 0; i < currMaxFD + 1; i++) {
      if (!(FD_ISSET(i, &currSet))) {
        continue;
      }

      if (i == mySockFD) {
        SocketFD newSockFD = accept(mySockFD, (struct sockaddr*) &clieAddrss, &clieAddrssLen);
        if (newSockFD < 0) {
          perror("accept()");
          continue;
        }
#if defined(DEBUG) && VERBOSENESS > 0
        Console::log(getUTCTime() + " [DEBUG] New TCP connection accepted! (socket: " + str(newSockFD) + ')');
#endif
        FD_SET(newSockFD, &nextSet);
        if (newSockFD > nextMaxFD) {
          nextMaxFD = newSockFD;
        }
      } else {
        // ***** RECEIVE BLOCK *****
        // Try to receive the header first...
        Buffer tcp_buffer(UDP_BUFFER_SIZE);
        int bytes = receive(i, tcp_buffer.data(), 1);
        if (bytes <= 0 ) {
          if (bytes < 0) {
            perror("recv()");
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
              // Nothing to read right now...
              continue;
            } else if (errno == EINTR) {
              // Interrupted, try later..
              continue;
            }
            // For all the other errno-s connection should be closed...
          }
          shutdown(SHUT_RDWR, i);
          close(i);
#if defined(DEBUG) && VERBOSENESS > 0
          Console::log(getUTCTime() + " [DEBUG] TCP connection closed! (socket: " + str(i) + ')');
#endif
          FD_CLR(i, &nextSet);
          if (i == nextMaxFD) {
            for (int j = 0; j < nextMaxFD + 1; j++) { // Update maximum file descriptor value...
              if (FD_ISSET(j, &nextSet)) {
                nextMaxFD = j;
              }
            }
          }
        } else {
          assert(bytes == 1);
          // Manage received message
          if (tcp_buffer[0] == ENTRY_REQUEST) {
            // This copy could be avoided...
            // TODO: check this!
            std::copy(tcp_buffer.data(), tcp_buffer.data() + 1, static_cast<uint8_t*>(request_segment.pointWritableBuffer()));
            tcp_buffer.assign(UDP_BUFFER_SIZE, 0);  // Reset content...
            ClientToken t = myManager.addClient();
            if (t < 0) {
              std::cerr << getUTCTime() << RED << " [ERROR] Impossible to add client!" << RESET << '\n';
              continue;
              // TODO Add error response!
            }
            request_segment.buildEntryResponse(t);
          } else if (tcp_buffer[0] == EXIT_REQUEST) {
            int bytes = receive(i, tcp_buffer.data() + 1, 2);
            if (bytes <= 0) {
              std::cerr << "Impossible to finalize exit request!" << '\n';
              continue;
            }
            assert(bytes == 2);
            std::copy(tcp_buffer.data(), tcp_buffer.data() + 3, static_cast<uint8_t*>(request_segment.pointWritableBuffer()));
            tcp_buffer.assign(UDP_BUFFER_SIZE, 0);  // Reset content...
            ClientToken t = request_segment.token();
            int res = myManager.removeClient(t);
            if (res < 0) {
              std::cerr << getUTCTime() << RED << " [ERROR] Impossible to remove client!" << RESET << '\n';
              // continue;
              // Send an OKAY response anyway because it's like to say: "okay, the token won't be/isn't here anymore"...
              // Maybe different response code is better.
            }
            request_segment.buildExitResponse();
          } else if (tcp_buffer[0] == AUDIO_STREAM_DATA) {
            int bytes = receive(i, tcp_buffer.data() + 1, 10);
            if (bytes <= 0) {
              std::cerr << "Impossible to finalize audio exchange request!" << '\n';
              continue;
            }
            assert(bytes == 10);
            TCPSegment::Size size = *(reinterpret_cast<TCPSegment::Size*>(tcp_buffer.data() + 7));
#ifdef DEBUG
            std::cout << "header: " << static_cast<uint>(*(reinterpret_cast<TCPSegment::Header*>(tcp_buffer.data()))) << std::endl;
            std::cout << "token: " << *(reinterpret_cast<ClientToken*>(tcp_buffer.data() + 1)) << std::endl;
            std::cout << "tid: " << *(reinterpret_cast<ClientTID*>(tcp_buffer.data() + 3)) << std::endl;
            std::cout << "size: " << size << std::endl;
            std::cout << "flag: " << static_cast<uint>(*(reinterpret_cast<TCPSegment::Flag*>(tcp_buffer.data() + 11 +  sizeof (AudioSample) * size))) << std::endl;
#endif
            assert((size == NUM_CHANNELS * AUDIO_VECTOR_SIZE) || (size == 0));
            bytes = receive(i, tcp_buffer.data() + 9, size * sizeof (AudioSample) + sizeof (TCPSegment::Flag));
            if (bytes <= 0) {
              std::cerr << "Impossible to finalize audio exchange request!" << '\n';
              continue;
            }
            assert(bytes == size * sizeof (AudioSample) + sizeof (TCPSegment::Flag));
            std::copy(tcp_buffer.data(), tcp_buffer.data() + UDP_BUFFER_SIZE, static_cast<uint8_t*>(request_segment.pointWritableBuffer()));
            tcp_buffer.assign(UDP_BUFFER_SIZE, 0);  // Reset content...
            myRequestInfo.setFileDescriptor(i);
            // myRequestInfo.setAddress(&clieAddrss, &clieAddrssLen);
            myRequestInfo.setReceiptTime(std::chrono::high_resolution_clock::now());
            myThreadPool.append(myRequestInfo);
#ifdef DEBUG
          Console::log("Audio stream request on queue.");
#endif
            continue; // Response send is managed in a separate thread, in this case...
          } else {
            std::cerr << getUTCTime() << RED << " [ERROR] Not consistent header (" << static_cast<uint8_t>(tcp_buffer[0]) << ") of TCP request!" << RESET << '\n';
            continue;
            // TODO Add error response!
          }
          int bytes = send(i, static_cast<const uint8_t*>(request_segment.rawBuffer()), request_segment.size());
          if (bytes < 0) {
            perror("send");
          }
#ifdef DEBUG
          Console::log("Response sent on the fly");
#endif
        }
        // ***** END OF RECEIVE BLOCK *****
      }
    }
    // ***** END OF SOCKET ITERATION BLOCK *****
    currSet = nextSet;
    timeout = T;
    currMaxFD = nextMaxFD; // Update maximum file descriptor value for the next cycle...
#ifdef DEBUG
    std::string s;
    s += ("currMaxFD: " + str(currMaxFD) + ", sockets: ");
    for (int k = 0; k < currMaxFD + 1; k++) {
      if (FD_ISSET(k, &currSet)) {
        s += (str(k) + ' ');
      }
    }
    Console::log(s);
#endif
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

int TCPListener::send(const SocketFD sfd, const uint8_t* buff, const size_t s) {
  return ::send(sfd, buff, s, 0);
}

int TCPListener::receive(const SocketFD sfd, uint8_t* buff, const size_t s) {
  uint total_bytes = 0;
  uint pointer_shift = 0;
  do {
    int bytes = recv(sfd, buff + pointer_shift, s - total_bytes, 0);
    if (bytes <= 0) { // if bytes is 0 means that the connection was closed...
      if (bytes < 0) {
        perror("receive()");
      }
      return bytes;
    }
    pointer_shift += bytes;
    total_bytes += bytes;
  } while (total_bytes < s);
  assert(total_bytes == s);
  return total_bytes;
}

bool TCPListener::listening() {
  std::lock_guard<std::mutex> l(myMutex);
  return active;
}
