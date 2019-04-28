#ifndef TCP_LISTENER
#define TCP_LISTENER

#include <arpa/inet.h>
#include <thread>
#include <vector>
#include "Manager.h"
#include "TCPSegment.h"
#include "types.h"

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
  TCPSegment mySegment;
  Manager& myManager;
  // ********************
  bool active;
  std::mutex myMutex;
  std::thread myThread;
};

#endif
