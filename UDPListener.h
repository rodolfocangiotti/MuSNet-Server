#ifndef UDP_LISTENER_H
#define UDP_LISTENER_H

#include <arpa/inet.h>
#include <thread>
#include <vector>
#include "RequestInfo.h"
#include "ThreadPool.h"
#include "UDPResponse.h"
#include "types.h"

class UDPListenerException: public std::exception {
public:
  UDPListenerException(const std::string e) noexcept;
  virtual ~UDPListenerException();
  virtual const char* what() const noexcept;
private:
  std::string error;
};

class UDPListener {
public:
  UDPListener(ThreadPool<UDPResponse>& tp);
  ~UDPListener();
  // ********************
  void configure(const PortNum pn);
  void start();
  void stop();
private:
  void initSocket();
  void bindSocket(const PortNum pn);
  void initClientAddress();
  void listen();
  int receive(void* buff, const size_t s);
  int receiveWithTimeout(fd_set* fds, void* buff, const size_t s);
  // ********************
  bool listening();
  // ********************
  SocketFD mySockFD;
  struct sockaddr_in myAddrss, clieAddrss;
  socklen_t myAddrssLen, clieAddrssLen;
  struct timeval myTimeout;
  // ********************
  RequestInfo myRequestInfo;
  ThreadPool<UDPResponse>& myThreadPool;
  // ********************
  bool active;
  std::mutex myMutex;
  std::thread myThread;
};

#endif
