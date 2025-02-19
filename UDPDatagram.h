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
  UDPDatagram(const MaxSize ms);
  ~UDPDatagram();
  // ********************
  ClientToken token() const;
  ClientTID tid() const;
  StreamSize streamSize() const;
  AudioVector streamCopy() const;
  // ********************
  void buildAudioStream(const ClientToken t, const ClientTID tid, const AudioVector& v);
};

#endif
