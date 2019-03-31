#ifndef UDP_LISTENER_H
#define UDP_LISTENER_H

#include <arpa/inet.h>
#include <thread>
#include <vector>
#include "RequestInfo.h"
#include "ThreadPool.h"
#include "UDPResponse.h"
#include "types.h"

class UDPListener {
public:
  UDPListener(ThreadPool<UDPResponse>& tp);
  ~UDPListener();
  void configure(PortNum p);
  void start();
  void stop();
private:
  void initSocket();
  void bindSocket(PortNum p);
  void initClientAddress();
  void listen();
  int receive(void* buff, size_t s);
  int receiveWithTimeout(void* buff, size_t s);
  // ********************
  bool listening();
  // ********************
  SocketFD mySockFD;
  struct sockaddr_in myAddrss, clieAddrss;
  socklen_t myAddrssLen, clieAddrssLen;
  // ********************
  RequestInfo myRequestInfo;
  ThreadPool<UDPResponse>& myThreadPool;
  // ********************
  bool active;
  std::mutex myMutex;
  std::thread myThread;
};

#endif
