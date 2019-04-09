#include <cassert>
#include <iostream>
#include "UDPDatagram.h"
#include "commons.h"

UDPDatagram::UDPDatagram(MaxSize s):
  Payload(s) {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing UDPDatagram class..." << std::endl;
#endif
}

UDPDatagram::~UDPDatagram() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing UDPDatagram class..." << std::endl;
#endif
}

StreamClient::Token UDPDatagram::token() const {
  const Byte* b = &(myBuff[1]);
  const StreamClient::Token* tp = reinterpret_cast<const StreamClient::Token*>(b);
  return *tp;
}

StreamClient::TID UDPDatagram::tid() const {
  const Byte* b = &(myBuff[3]);
  const StreamClient::TID* tidp = reinterpret_cast<const StreamClient::TID*>(b);
  return *tidp;
}

UDPDatagram::StreamSize UDPDatagram::streamSize() const {
  const Byte* b = &(myBuff[7]);
  const StreamSize* ssp = reinterpret_cast<const StreamSize*>(b);
  return *ssp;
}

AudioVector UDPDatagram::streamCopy() const { // This method returns a copy of the audio streaming content...
  const Byte* b = &(myBuff[7]);
  const StreamSize* ssp = reinterpret_cast<const StreamSize*>(b);
  b = &(myBuff[9]);
  const AudioSample* asp = reinterpret_cast<const AudioSample*>(b);
  AudioVector v(*ssp, 0.0);
  for (int i = 0; i < v.size(); i++) {
    v[i] = asp[i];
  }
  return v;
}

void UDPDatagram::buildAudioStream(AudioVector& v, StreamClient::Token t, StreamClient::TID tid) {
  mySize = sizeof (Header) + sizeof (StreamClient::Token) + sizeof(StreamClient::TID) + sizeof (Size) + sizeof (AudioSample) * v.size();
  Byte* b = &(myBuff[0]);
  Header* hp = static_cast<Header*>(b);
  *hp = AUDIO_STREAM_DATA;
  b = &(myBuff[1]);
  StreamClient::Token* tp = reinterpret_cast<StreamClient::Token*>(b);
  *tp = t;
  b = &(myBuff[3]);
  StreamClient::TID* tidp = reinterpret_cast<StreamClient::TID*>(b);
  *tidp = tid;
  b = &(myBuff[7]);
  StreamSize* ssp = reinterpret_cast<StreamSize*>(b);
  *ssp = v.size();
  b = &(myBuff[9]);
  AudioSample* asp = reinterpret_cast<AudioSample*>(b);
  for (int i = 0; i < v.size(); i++) {
    asp[i] = v[i];
  }
}
