#include <iostream>
#include "TCPSegment.h"
#include "commons.h"

TCPSegment::TCPSegment(MaxSize ms):
  Payload(ms) {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing TCPSegment class..." << std::endl;
#endif
}

TCPSegment::~TCPSegment() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing TCPSegment class..." << std::endl;
#endif
}

StreamClient::Token TCPSegment::token() const {
  const Byte* bp = &(myBuff[1]);
  const StreamClient::Token* tp = reinterpret_cast<const StreamClient::Token*>(bp);
  return *tp;
}

void TCPSegment::buildEntryRequest() {
  mySize = sizeof (Header);
  Byte* bp = &(myBuff[0]);
  Header* hp = reinterpret_cast<Header*>(bp);
  *hp = ENTRY_REQUEST;
}

void TCPSegment::buildEntryResponse(StreamClient::Token t) {
  mySize = sizeof (Header) + sizeof (StreamClient::Token);
  Byte* bp = &(myBuff[0]);
  Header* hp = reinterpret_cast<Header*>(bp);
  *hp = OKAY;
  bp = &(myBuff[1]);
  StreamClient::Token* tp = reinterpret_cast<StreamClient::Token*>(bp);
  *tp = t;
}

void TCPSegment::buildExitRequest(StreamClient::Token t) {
  mySize = sizeof (Header) + sizeof (StreamClient::Token);
  Byte* bp = &(myBuff[0]);
  Header* hp = reinterpret_cast<Header*>(bp);
  *hp = EXIT_REQUEST;
  bp = &(myBuff[1]);
  StreamClient::Token* tp = reinterpret_cast<StreamClient::Token*>(bp);
  *tp = t;
}

void TCPSegment::buildExitResponse() {
  mySize = sizeof (Header);
  Byte* bp = &(myBuff[0]);
  Header* hp = reinterpret_cast<Header*>(bp);
  *hp = OKAY;
}
