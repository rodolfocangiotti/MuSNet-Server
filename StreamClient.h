#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include <cstdint>
#include <vector>
#include "types.h"
//#include "SharedBuffer.h"

class StreamClient {
public:
  typedef uint32_t ID;
  typedef uint32_t Token;
  // ********************
  StreamClient(const Token t);
  ~StreamClient();
  // ********************
  const AudioVector& audioVector() const;
  Token token() const;
  ID id() const;
  // ********************
  void* pointWritableVector();
  void clearVector();
  void updateVector(AudioVector v);
  void updateID(ID i);
private:
  AudioVector myAudioVector;
  ID myID;
  Token myToken;
};

#endif
