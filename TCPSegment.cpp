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
  const Byte* b = &(myBuff[1]);
  const StreamClient::Token* pt = reinterpret_cast<const StreamClient::Token*>(b);
  return *pt;
}

void TCPSegment::buildEntryRequest() {
  mySize = sizeof (Header);
  Byte* b = &(myBuff[0]);
  Header* ph = reinterpret_cast<Header*>(b);
  *ph = ENTRY_REQUEST;
}

void TCPSegment::buildEntryResponse(StreamClient::Token t) {
  mySize = sizeof (Header) + sizeof (StreamClient::Token);
  Byte* b = &(myBuff[0]);
  Header* ph = reinterpret_cast<Header*>(b);
  *ph = OKAY;
  b = &(myBuff[1]);
  StreamClient::Token* pt = reinterpret_cast<StreamClient::Token*>(b);
  *pt = t;
}

void TCPSegment::buildExitRequest(StreamClient::Token t) {
  mySize = sizeof (Header) + sizeof (StreamClient::Token);
  Byte* b = &(myBuff[0]);
  Header* ph = reinterpret_cast<Header*>(b);
  *ph = EXIT_REQUEST;
  b = &(myBuff[1]);
  StreamClient::Token* pt = reinterpret_cast<StreamClient::Token*>(b);
  *pt = t;
}

void TCPSegment::buildExitResponse() {
  mySize = sizeof (Header);
  Byte* b = &(myBuff[0]);
  Header* ph = reinterpret_cast<Header*>(b);
  *ph = OKAY;
}
