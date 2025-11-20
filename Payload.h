#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <cstdint>
#include <vector>
#include "types.h"

class Payload {
public:
  typedef uint8_t Header;
  typedef uint32_t MaxSize;
  typedef uint32_t Size;
  // ********************
  Payload(const MaxSize ms);
  ~Payload();
  // ********************
  const Buffer& buffer() const;
  const void* rawBuffer() const;
  Header header() const;
  Size size() const;
  // ********************
  void* pointWritableBuffer();
protected:
  Buffer myBuff;
  Size mySize;
};

#endif
