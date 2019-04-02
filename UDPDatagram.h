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
  typedef uint16_t Token;
  typedef uint32_t ID;
  // ********************
  UDPDatagram(MaxSize ms);
  ~UDPDatagram();
  // ********************
  Token token() const;
  ID id() const;
  StreamSize streamSize() const;
  AudioVector streamCopy() const;
  // ********************
  //void buildAudioStream(AudioVector& v, Mode m);
  void buildAudioStreamResponse(AudioVector& v, Token t, ID i);
protected:
  void setStreamSize(StreamSize ss);
  void setStreamContent(AudioVector& v);
};

#endif
