#ifndef TCP_SEGMENT_H
#define TCP_SEGMENT_H

#include <cstdint>
#include "Payload.h"

class TCPSegment: public Payload {
public:
  typedef uint16_t Token;
  TCPSegment(MaxSize ms);
  ~TCPSegment();
  // ********************
  Token token() const;
  // ********************
  void buildEntryRequest();
  void buildEntryResponse(Token t);
  void buildExitRequest(Token t);
  void buildExitResponse();
};

#endif
