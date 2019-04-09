#include <iostream>
#include "Payload.h"
#include "commons.h"

// TODO Check if it should be better to compact the code, avoting repetition...

Payload::Payload(MaxSize ms):
  myBuff(ms, 0), mySize(0) {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing Payload class..." << std::endl;
#endif
}

Payload::~Payload() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing Payload class..." << std::endl;
#endif
}

const Buffer& Payload::buffer() const {
  return myBuff;
}

const void* Payload::rawBuffer() const {
  return myBuff.data();
}

Payload::Header Payload::header() const {
  const Byte* b = &(myBuff[0]);
  const Header* h = reinterpret_cast<const Header*>(b);
  return *h;
}

Payload::Size Payload::size() const {
  return mySize;
}

void* Payload::pointWritableBuffer() {
  return myBuff.data();
}
