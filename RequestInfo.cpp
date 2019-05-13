#include <iostream>
#include "RequestInfo.h"

RequestInfo::RequestInfo(BufferSize bs):
  myDatagram(bs) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing RequestInfo class..." << '\n';
#endif
}

RequestInfo::~RequestInfo() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing RequestInfo class..." << '\n';
#endif
}

void RequestInfo::setAddress(const struct sockaddr_in* addrss, const socklen_t* addrssLen) {
  myAddrss = *addrss;
  myAddrssLen = *addrssLen;
}
void RequestInfo::setFileDescriptor(SocketFD s) {
  mySockFD = s;
}

struct sockaddr_in RequestInfo::address() const {
  return myAddrss;
}

socklen_t RequestInfo::addressLength() const {
  return myAddrssLen;
}

SocketFD RequestInfo::fileDescriptor() const {
  return mySockFD;
}

const UDPDatagram& RequestInfo::referDatagram() const {
  return myDatagram;
}

UDPDatagram& RequestInfo::referWritableDatagram() {
  return myDatagram;
}
