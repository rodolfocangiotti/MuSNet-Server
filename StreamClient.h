#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include <cstdint>
#include <list>
#include "StreamVector.h"
#include "types.h"

bool comp(const StreamVector& a, const StreamVector& b);

class StreamClient {
public:
  /*
  typedef int16_t Token;  // XXX These data types have been moved to types.h...
  typedef int16_t TID;
  // ********************
  */
  StreamClient(const ClientToken t);
  ~StreamClient();
  // ********************
  ClientToken token() const;
  // ********************
  ClientTID getNewResponseTID();
  // ********************
  int addReader(const ClientToken ot);
  int removeReader(const ClientToken ot);
  // ********************
  AudioVector retrieveVector(const ClientToken ot);
  void insertVector(const ClientToken mt, const ClientTID wtid, const AudioVector& v);
private:
  typedef std::list<StreamVector> StreamQueue;
  typedef std::list<ClientToken> ReadManager;
  // ********************
  StreamQueue myQueue;
  ReadManager myReaders;
  ClientTID myLastResp;  // Last TID sent to client for fetch/download requests...
  ClientToken myToken;  // Client code...
  // ********************
  int myQueueMaxSize;
};

#endif
