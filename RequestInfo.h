#ifndef REQUEST_INFO
#define REQUEST_INFO

#include <arpa/inet.h>
#include <cstdint>
#include "types.h"

class RequestInfo {
public:
  RequestInfo();
  ~RequestInfo();
  // ********************
  void setAddress(const struct sockaddr_in* addrss, const socklen_t* addrssLen);
  void setFileDescriptor(const SocketFD s);
  void setReceiptTime(const HighResolutionTime& e);
  // ********************
  struct sockaddr_in address() const;
  socklen_t addressLength() const;
  SocketFD fileDescriptor() const;
  HighResolutionTime receiptTime() const;
private:
  struct sockaddr_in myAddrss;
  socklen_t myAddrssLen;
  SocketFD mySockFD;
  HighResolutionTime myRecptTime;
};

#endif
