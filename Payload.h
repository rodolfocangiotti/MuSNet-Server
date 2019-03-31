#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <cstdint>
#include <vector>

class Payload {
public:
  typedef uint8_t Byte;
  typedef std::vector<Byte> Buffer;
  typedef uint8_t Header;
  typedef uint16_t MaxSize;
  typedef uint16_t Size;
  // ********************
  Payload(MaxSize ms);
  ~Payload();
  Payload(const Payload& other) = delete; // TODO Test copy constructor...
  // ********************
  const void* buffer() const;
  Header header() const;
  Size size() const;
  // ********************
  void* pointWritableBuffer();
protected:
  Buffer myBuff;
  Size mySize;
};

#endif
