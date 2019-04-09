#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include <cstdint>
#include <vector>
#include "types.h"

class StreamClient {
public:
  typedef uint16_t Token;
  typedef uint32_t TID;
  // ********************
  StreamClient(Token t);
  ~StreamClient();
  // ********************
  const AudioVector& audioVector() const;
  Token token() const;
  TID tid() const;
  // ********************
  void* pointWritableVector();
  void clearVector();
  void updateVector(AudioVector v);
  void updateTID(TID tid);
private:
  AudioVector myAudioVect;
  TID myTID;  // Transaction identifier...
  Token myToken;
};

#endif
