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
/*
void UDPDatagram::buildAudioStreamRequest() {
  mySize = sizeof (Header) + sizeof (Token);
  Header* head = static_cast<Header*>(myBuff.data());
  *head = AUDIO_STREAM_REQUEST;
  Token* t = reinterpret_cast<Token*>(++head);
  *t = myToken;
}

void UDPDatagram::buildAudioStreamData(Memory::SharedBuffer<double>& sb) {
  mySize = sizeof (Header) + sizeof (Token) + sizeof (Size) + sizeof (AudioSample) * sb.size();
  Byte* b = myBuff.data();
  Header* h = static_cast<Header*>(b);
  *h = AUDIO_STREAM_DATA;
  b += sizeof (Header);
  Token* t = reinterpret_cast<Token*>(b);
  *t = myToken;
  b += sizeof (Token);
  Size* s = reinterpret_cast<Size*>(b);
  *s = sb.size();
  b += sizeof (Size);
  AudioSample* values = reinterpret_cast<AudioSample*>(b);
  for (int i = 0; i < sb.size(); i++) {
    values[i] = sb[i];
  }
}

Size UDPDatagram::streamSize() {  // This method returns the size of the audio streaming vector...
  Byte* b = myBuff.data();
  b += sizeof (Header);
  Token* t = reinterpret_cast<Token*>(b);
  *t = myToken;
  b += sizeof (Token);
  Size* s = reinterpret_cast<Size*>(b);
  return *s;
}

AudioVector UDPDatagram::streamCopy() { // This method returns a copy of the audio streaming content...
  Byte* b = myBuff.data();
  b += sizeof (Header);
  Token* t = reinterpret_cast<Token*>(b);
  *t = myToken;
  b += sizeof (Token);
  Size* s = reinterpret_cast<Size*>(b);
  b += sizeof (Size);
  AudioSample* values = reinterpret_cast<AudioSample*>(b);
  AudioVector v(*s);
  for (int i = 0; i < v.size(); i++) {
    v[i] = values[i];
  }
  return v;
}
*/
