#ifndef TCP_SEGMENT_H
#define TCP_SEGMENT_H

#include <cstdint>
#include "Payload.h"
#include "types.h"

class TCPSegment: public Payload {
public:
  TCPSegment(const MaxSize ms);
  ~TCPSegment();
  // ********************
  ClientToken token() const;
  // ********************
  void buildEntryRequest();
  void buildEntryResponse(const ClientToken t);
  void buildExitRequest(const ClientToken t);
  void buildExitResponse();
};

#endif
