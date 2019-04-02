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

UDPDatagram::Token UDPDatagram::token() const {
  const Byte* b = &(myBuff[1]);
  const Token* tp = reinterpret_cast<const Token*>(b);
  return *tp;
}

UDPDatagram::ID UDPDatagram::id() const {
  const Byte* b = &(myBuff[5]);
  const ID* ip = reinterpret_cast<const ID*>(b);
  return *ip;
}

UDPDatagram::StreamSize UDPDatagram::streamSize() const {
  const Byte* b = &(myBuff[13]);
  const StreamSize* ssp = reinterpret_cast<const StreamSize*>(b);
  return *ssp;
}

AudioVector UDPDatagram::streamCopy() const { // This method returns a copy of the audio streaming content...
  const Byte* b = &(myBuff[13]);
  const StreamSize* ssp = reinterpret_cast<const StreamSize*>(b);
  b = &(myBuff[15]);
  const AudioSample* asp = reinterpret_cast<const AudioSample*>(b);
  AudioVector v(*ssp);
  for (int i = 0; i < v.size(); i++) {
    v[i] = asp[i];
  }
  return v;
}

/*
void UDPDatagram::buildAudioStreamRequest() {
  mySize = sizeof (Header) + sizeof (Token);
  Header* head = static_cast<Header*>(myBuff.data());
  *head = AUDIO_STREAM_REQUEST;
  Token* t = reinterpret_cast<Token*>(++head);
  *t = myToken;
}
*/

void UDPDatagram::buildAudioStreamResponse(AudioVector& v, Token t, ID i) {
  mySize = sizeof (Header) + sizeof (Token) + sizeof(ID) + sizeof (Size) + sizeof (AudioSample) * v.size();
  Byte* b = &(myBuff[0]);
  Header* hp = static_cast<Header*>(b);
  *hp = AUDIO_STREAM_DATA;
  b = &(myBuff[1]);
  Token* tp = reinterpret_cast<Token*>(b);
  *tp = t;
  b = &(myBuff[5]);
  ID* ip = reinterpret_cast<ID*>(b);
  *ip = i;
  b = &(myBuff[13]);
  Size* sp = reinterpret_cast<Size*>(b);
  *sp = v.size();
  b = &(myBuff[15]);
  AudioSample* asp = reinterpret_cast<AudioSample*>(b);
  for (int i = 0; i < v.size(); i++) {
    asp[i] = v[i];
  }
}
