#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include <cstdint>
#include "SharedBuffer.h"

class StreamClient {
public:
  typedef uint32_t ID;
  typedef uint32_t Token;
  // ********************
  StreamClient(const Token t);
  ~StreamClient();
  // ********************
  const SharedBuffer& sharedBuffer() const;
  Token token() const;
  ID lastID() const;
  // ********************
  void updateID(ID i);
  SharedBuffer* pointSharedBuffer();
private:
  ID myLastID;
  SharedBuffer myBuffer;
  Token myToken;
};

#endif
