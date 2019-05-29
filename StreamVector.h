#ifndef STREAM_VECTOR_H
#define STREAM_VECTOR_H

#include <map>
#include "types.h"

class StreamVector {
public:
  StreamVector();
  ~StreamVector();
  // ********************
  ClientToken owner() const;
  ClientTID tid() const;
  // ********************
  void setAudioVector(const AudioVector& v);
  void setOwner(const ClientToken t);
  void setTID(const ClientTID tid);
  // ********************
  int addReadPermission(const ClientToken t);
  int removeReadPermission(const ClientToken t);
  // ********************
  AudioVector readVector(const ClientToken t);
  // ********************
  bool isDeletable();
  bool isReadableBy(const ClientToken t);
private:
  typedef std::map<ClientToken, bool> ReadLogger;
  // ********************
  AudioVector myVect;
  ReadLogger myLog;
  ClientToken myOwner;
  ClientTID myTID;
};

#endif
