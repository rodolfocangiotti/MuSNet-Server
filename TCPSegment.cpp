#include <cassert>
#include <iostream>
#include "TCPSegment.h"
#include "commons.h"
#include "utils.h"

TCPSegment::TCPSegment(const MaxSize ms):
  Payload(ms) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Constructing TCPSegment class..." << '\n';
#endif
}

TCPSegment::~TCPSegment() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Destructing TCPSegment class..." << '\n';
#endif
}

ClientToken TCPSegment::token() const {
  const Byte* bp = &(myBuff[1]);
  const ClientToken* tp = reinterpret_cast<const ClientToken*>(bp);
  return *tp;
}

void TCPSegment::buildEntryRequest() {
  mySize = sizeof (Header);
  assert(mySize <= myBuff.size());
  Byte* bp = &(myBuff[0]);
  Header* hp = reinterpret_cast<Header*>(bp);
  *hp = ENTRY_REQUEST;
}

void TCPSegment::buildEntryResponse(const ClientToken t) {
  mySize = sizeof (Header) + sizeof (ClientToken);
  assert(mySize <= myBuff.size());
  Byte* bp = &(myBuff[0]);
  Header* hp = reinterpret_cast<Header*>(bp);
  *hp = OKAY_RESPONSE;
  bp = &(myBuff[1]);
  ClientToken* tp = reinterpret_cast<ClientToken*>(bp);
  *tp = t;
}

void TCPSegment::buildExitRequest(const ClientToken t) {
  mySize = sizeof (Header) + sizeof (ClientToken);
  assert(mySize <= myBuff.size());
  Byte* bp = &(myBuff[0]);
  Header* hp = reinterpret_cast<Header*>(bp);
  *hp = EXIT_REQUEST;
  bp = &(myBuff[1]);
  ClientToken* tp = reinterpret_cast<ClientToken*>(bp);
  *tp = t;
}

void TCPSegment::buildExitResponse() {
  mySize = sizeof (Header);
  assert(mySize <= myBuff.size());
  Byte* bp = &(myBuff[0]);
  Header* hp = reinterpret_cast<Header*>(bp);
  *hp = OKAY_RESPONSE;
}
