#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include <algorithm>
#include <cstdint>
#include <list>
//#include <map>
//#include <vector>
//#include "SharedVector.h"
#include "StreamVector.h"
#include "types.h"

bool comp(const StreamVector& a, const StreamVector& b);

class StreamClient {
public:
  /*
  typedef int16_t Token;  // XXX These data types have been moved to types.h...
  typedef int16_t TID;
  */
  StreamClient(ClientToken t);
  ~StreamClient();
  // ********************
  /*
  ClientTID maximumWriteTID() const;
  ClientTID minimumWriteTID() const;
  */
  ClientToken token() const;
  // ********************
  ClientTID getNewResponseTID();
  // ********************
  int addReader(ClientToken ot);
  int removeReader(ClientToken ot);
  // ********************
  AudioVector retrieveVector(ClientToken ot);
  void insertVector(ClientToken mt, ClientTID wtid, AudioVector& v);
private:
  typedef std::map<ClientToken, ClientTID> ReadHistory;
  typedef std::vector<ClientTID> WriteHistory;
  typedef std::list<StreamVector> StreamQueue;
  typedef std::vector<ClientToken> ReadManager;
  // ********************
  /*
  void debugPrintVIDLimits(ClientVID vid);
  void debugPrintReadStatus();
  // ********************
  void updateReadHistory(ClientToken ot, ClientTID rtid);
  void updateWriteHistory(ClientTID wtid);
  */
  // ********************
  StreamQueue myQueue;
  ReadManager myReaders;
  ClientTID myLastResp;  // Last TID sent to client for fetch/download requests...
  //ClientTID myLastWReq; // Last TID received from client for update requests...
  ClientToken myToken;  // Client code...
  /*
  ReadHistory myRReqsts;  // Data structure containing the last TID received from each other client for fetch/download requests...
  WriteHistory myWReqsts; // Data structure containing a history of TIDs received from clients for update requests...
  */
};

#endif
