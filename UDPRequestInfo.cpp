#include <iostream>
#include "Console.h"
#include "UDPRequestInfo.h"
#include "utils.h"

UDPRequestInfo::UDPRequestInfo(const BufferSize bs):
  _datagram(bs) {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing UDPRequestInfo class...");
#endif
}

UDPRequestInfo::~UDPRequestInfo() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Destructing UDPRequestInfo class...");
#endif
}

const UDPDatagram& UDPRequestInfo::referDatagram() const {
  return _datagram;
}

UDPDatagram& UDPRequestInfo::referWritableDatagram() {
  return _datagram;
}
