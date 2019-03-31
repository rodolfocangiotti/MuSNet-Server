#ifndef UDP_DATAGRAM_H
#define UDP_DATAGRAM_H

#include <cstdint>
#include "Payload.h"
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
  StreamSize streamSize() const;
  AudioVector streamCopy() const;
  // ********************
  void buildAudioStream(AudioVector& v, Mode m);
protected:
  void setStreamSize(StreamSize ss);
  void setStreamContent(AudioVector& v);
};

#endif
