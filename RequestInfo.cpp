#include <iostream>
#include "Console.h"
#include "RequestInfo.h"
#include "utils.h"

RequestInfo::RequestInfo(const BufferSize bs):
  myAddrss(), myAddrssLen(0),
  mySockFD(0),
  myDatagram(bs),
  myRecptTime() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing RequestInfo class...");
#endif
}

RequestInfo::~RequestInfo() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Destructing RequestInfo class...");
#endif
}

void RequestInfo::setAddress(const struct sockaddr_in* addrss, const socklen_t* addrssLen) {
  myAddrss = *addrss;
  myAddrssLen = *addrssLen;
}
void RequestInfo::setFileDescriptor(const SocketFD s) {
  mySockFD = s;
}

void RequestInfo::setReceiptTime(const Time& e) {
  myRecptTime = e;
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

Time RequestInfo::receiptTime() const {
  return myRecptTime;
}

const UDPDatagram& RequestInfo::referDatagram() const {
  return myDatagram;
}

UDPDatagram& RequestInfo::referWritableDatagram() {
  return myDatagram;
}
