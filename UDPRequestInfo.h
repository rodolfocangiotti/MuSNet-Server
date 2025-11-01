#ifndef UDP_REQUEST_INFO
#define UDP_REQUEST_INFO

#include <arpa/inet.h>
#include <cstdint>
#include "RequestInfo.h"
#include "UDPDatagram.h"
#include "types.h"

class UDPRequestInfo: public RequestInfo {
public:
  typedef uint64_t BufferSize;
  // ********************
  UDPRequestInfo(const BufferSize bs);
  ~UDPRequestInfo();
  // ********************
  const UDPDatagram& referDatagram() const;
  UDPDatagram& referWritableDatagram();
private:
  UDPDatagram _datagram;
};

#endif
