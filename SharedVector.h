#ifndef SHARED_VECTOR_H
#define SHARED_VECTOR_H

#include <map>
#include "types.h"

class SharedVector {
public:
  SharedVector();
  ~SharedVector();
  // ********************
  void setOwner(ClientToken t);
  void setVID(ClientVID vid);
  // ********************
  int addReader(ClientToken t);
  int removeReader(ClientToken t);
  // ********************
  void clear();
  AudioVector read(ClientToken t);
  void write(AudioVector& v);
  // ********************
  bool isToClear();
private:
  typedef std::map<ClientToken, int8_t> ReadLogger;
  // ********************
  void printReadStatus();
  // ********************
  ClientToken myOwner;
  ClientVID myVID;
  // ********************
  bool cleared;
  ReadLogger myLog;
  AudioVector myVect;
};

#endif
