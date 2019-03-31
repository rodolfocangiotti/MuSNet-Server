#ifndef REQUEST_INFO
#define REQUEST_INFO

#include <arpa/inet.h>
#include <cstdint>
#include <vector>

typedef uint8_t Byte;
typedef std::vector<Byte> Buffer;
typedef uint64_t BufferSize;

class RequestInfo {
public:
  RequestInfo(BufferSize s);
  ~RequestInfo();
  // ********************
  void* pointWritableBuffer();
  void setAddress(const struct sockaddr_in* addrss, const socklen_t* addrssLen);
  // ********************
  struct sockaddr_in address() const;
  socklen_t addressLength() const;
  const void* buffer() const;
private:
  struct sockaddr_in myAddrss;
  socklen_t myAddrssLen;
  // ********************
  Buffer myBuffer;
};

#endif
