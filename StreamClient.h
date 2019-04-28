#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include <cstdint>
#include <map>
#include <vector>
#include "types.h"

class StreamClient {
public:
  typedef int16_t Token;
  typedef int32_t TID;
  // ********************
  StreamClient(Token t);
  ~StreamClient();
  // ********************
  Token token() const;
  TID tid() const;
  // ********************
  void clearVector();
  AudioVector readVector(Token t);
  void writeVector(AudioVector& v);
  void updateTID(TID tid);
  // ********************
  int addReader(Token t);
  int removeReader(Token t);
private:
  typedef AudioVector AudioBuffer;  // Local alias of AudioVector...
  typedef uint64_t Index;
  typedef std::map<Token, Index> AccessManager;
  AudioBuffer myBuff;
  AccessManager myReaders;
  Index myIndx;
  TID myTID;  // Transaction identifier...
  Token myToken;
};

#endif
