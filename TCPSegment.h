#ifndef TCP_SEGMENT_H
#define TCP_SEGMENT_H

#include <cstdint>
#include "Payload.h"
#include "StreamClient.h"

class TCPSegment: public Payload {
public:
  TCPSegment(MaxSize ms);
  ~TCPSegment();
  // ********************
  StreamClient::Token token() const;
  // ********************
  void buildEntryRequest();
  void buildEntryResponse(StreamClient::Token t);
  void buildExitRequest(StreamClient::Token t);
  void buildExitResponse();
};

#endif
