#ifndef UDP_DATAGRAM_H
#define UDP_DATAGRAM_H

#include <cstdint>
#include "Payload.h"
#include "StreamClient.h"
#include "commons.h"
#include "types.h"

class UDPDatagram: public Payload {
public:
  typedef uint8_t Mode;
  typedef uint16_t StreamSize;
  // ********************
  UDPDatagram(MaxSize ms);
  ~UDPDatagram();
  // ********************
  StreamClient::Token token() const;
  StreamClient::TID tid() const;
  StreamSize streamSize() const;
  AudioVector streamCopy() const;
  // ********************
  void buildAudioStream(AudioVector& v, StreamClient::Token t, StreamClient::TID tid);
protected:
  void setStreamSize(StreamSize ss);
  void setStreamContent(AudioVector& v);
};

#endif
