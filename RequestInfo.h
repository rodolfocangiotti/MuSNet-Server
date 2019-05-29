#ifndef REQUEST_INFO
#define REQUEST_INFO

#include <arpa/inet.h>
#include <cstdint>
#include <vector>
#include "UDPDatagram.h"
#include "types.h"

class RequestInfo {
public:
  typedef uint64_t BufferSize;
  // ********************
  RequestInfo(const BufferSize bs);
  ~RequestInfo();
  // ********************
  void setAddress(const struct sockaddr_in* addrss, const socklen_t* addrssLen);
  void setFileDescriptor(const SocketFD s);
  void setReceiptTime(const Time& e);
  // ********************
  struct sockaddr_in address() const;
  socklen_t addressLength() const;
  SocketFD fileDescriptor() const;
  Time receiptTime() const;
  // ********************
  const UDPDatagram& referDatagram() const;
  UDPDatagram& referWritableDatagram();
private:
  struct sockaddr_in myAddrss;
  socklen_t myAddrssLen;
  SocketFD mySockFD;
  // ********************
  UDPDatagram myDatagram;
  // ********************
  Time myRecptTime;
};

#endif
