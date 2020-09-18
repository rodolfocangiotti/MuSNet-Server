#include <iostream>
#include "Console.h"
#include "Payload.h"
#include "commons.h"
#include "utils.h"

// TODO Check if it should be better to compact the code, avoiding repetition...

Payload::Payload(const MaxSize ms):
  myBuff(ms, 0),
  mySize(0) {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing Payload class...");
#endif
}

Payload::~Payload() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Destructing Payload class...");
#endif
}

const Buffer& Payload::buffer() const {
  return myBuff;
}

const void* Payload::rawBuffer() const {
  return myBuff.data();
}

Payload::Header Payload::header() const {
  const Byte* bp = &(myBuff[0]);
  const Header* hp = reinterpret_cast<const Header*>(bp);
  return *hp;
}

Payload::Size Payload::size() const {
  return mySize;
}

void* Payload::pointWritableBuffer() {
  return myBuff.data();
}
