#include <iostream>
#include "RequestInfo.h"

RequestInfo::RequestInfo(BufferSize bs):
  myBuff(bs) {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing RequestInfo class..." << std::endl;
#endif
}

RequestInfo::~RequestInfo() {
#ifdef DEBUG
  /*
  std::cout << "[DEBUG] Destructing RequestInfo class..." << std::endl;
  */
#endif
}

void RequestInfo::setAddress(const struct sockaddr_in* addrss, const socklen_t* addrssLen) {
  myAddrss = *addrss;
  myAddrssLen = *addrssLen;
}
void RequestInfo::setFileDescriptor(SocketFD s) {
  mySockFD = s;
}

void* RequestInfo::pointWritableBuffer() {
  return myBuff.data();
}

struct sockaddr_in RequestInfo::address() const {
  return myAddrss;
}

socklen_t RequestInfo::addressLength() const {
  return myAddrssLen;
}

const Buffer& RequestInfo::buffer() const {
  return myBuff;
}

SocketFD RequestInfo::fileDescriptor() const {
  return mySockFD;
}
