#ifndef REQUEST_INFO
#define REQUEST_INFO

#include <arpa/inet.h>
#include <cstdint>
#include <vector>
#include "types.h"

class RequestInfo {
public:
  typedef uint64_t BufferSize;
  RequestInfo(BufferSize s);
  ~RequestInfo();
  // ********************
  void* pointWritableBuffer();
  void setAddress(const struct sockaddr_in* addrss, const socklen_t* addrssLen);
  void setFileDescriptor(SocketFD s);
  // ********************
  struct sockaddr_in address() const;
  socklen_t addressLength() const;
  const Buffer& buffer() const;
  SocketFD fileDescriptor() const;
private:
  struct sockaddr_in myAddrss;
  socklen_t myAddrssLen;
  SocketFD mySockFD;
  // ********************
  Buffer myBuffer;
};

#endif
