#include <cassert>
#include <iostream>
#include "UDPDatagram.h"
#include "commons.h"

UDPDatagram::UDPDatagram(MaxSize ms):
  Payload(ms) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing UDPDatagram class..." << std::endl;
#endif
}

UDPDatagram::~UDPDatagram() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing UDPDatagram class..." << std::endl;
#endif
}

StreamClient::Token UDPDatagram::token() const {
  const Byte* bp = &(myBuff[1]);
  const StreamClient::Token* tp = reinterpret_cast<const StreamClient::Token*>(bp);
  return *tp;
}

StreamClient::TID UDPDatagram::tid() const {
  const Byte* bp = &(myBuff[3]);
  const StreamClient::TID* tidp = reinterpret_cast<const StreamClient::TID*>(bp);
  return *tidp;
}

UDPDatagram::StreamSize UDPDatagram::streamSize() const {
  const Byte* bp = &(myBuff[7]);
  const StreamSize* ssp = reinterpret_cast<const StreamSize*>(bp);
  return *ssp;
}

AudioVector UDPDatagram::streamCopy() const { // This method returns a copy of the audio streaming content...
  const Byte* bp = &(myBuff[7]);
  const StreamSize* ssp = reinterpret_cast<const StreamSize*>(bp);
  bp = &(myBuff[9]);
  const AudioSample* asp = reinterpret_cast<const AudioSample*>(bp);
  AudioVector v(*ssp);
  for (int i = 0; i < v.size(); i++) {
    v[i] = asp[i];
  }
  return v;
}

void UDPDatagram::buildAudioStream(AudioVector& v, StreamClient::Token t, StreamClient::TID tid) {
  mySize = sizeof (Header) + sizeof (StreamClient::Token) + sizeof (StreamClient::TID) + sizeof (Size) + sizeof (AudioSample) * v.size();
  Byte* bp = &(myBuff[0]);
  Header* hp = static_cast<Header*>(bp);
  *hp = AUDIO_STREAM_DATA;
  bp = &(myBuff[1]);
  StreamClient::Token* tp = reinterpret_cast<StreamClient::Token*>(bp);
  *tp = t;
  bp = &(myBuff[3]);
  StreamClient::TID* tidp = reinterpret_cast<StreamClient::TID*>(bp);
  *tidp = tid;
  bp = &(myBuff[7]);
  StreamSize* sp = reinterpret_cast<StreamSize*>(bp);
  *sp = v.size();
  bp = &(myBuff[9]);
  AudioSample* asp = reinterpret_cast<AudioSample*>(bp);
  for (int i = 0; i < v.size(); i++) {
    asp[i] = v[i];
  }
}
