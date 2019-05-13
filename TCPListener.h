#ifndef TCP_LISTENER_H
#define TCP_LISTENER_H

#include <arpa/inet.h>
#include <thread>
#include <vector>
#include "Manager.h"
#include "TCPSegment.h"
#include "types.h"

class TCPListenerException: public std::exception {
public:
  TCPListenerException(const std::string e) noexcept;
  virtual ~TCPListenerException();
  virtual const char* what() const noexcept;
private:
  std::string error;
};

class TCPListener {
public:
  TCPListener(Manager& m);
  ~TCPListener();
  // ********************
  void configure(const PortNum pn);
  void start();
  void stop();
private:
  void initSocket();
  void bindSocket(const PortNum pn);
  void initClientAddress();
  void listen();
  int send(SocketFD sfd, const void* buff, size_t s);
  int receive(SocketFD sfd, void* buff, size_t s);
  // ********************
  bool listening();
  // ********************
  SocketFD mySockFD;
  struct sockaddr_in myAddrss, clieAddrss;
  socklen_t myAddrssLen, clieAddrssLen;
  // ********************
  Manager& myManager;
  TCPSegment mySegment;
  // ********************
  bool active;
  std::mutex myMutex;
  std::thread myThread;
};

#endif
