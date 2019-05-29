#ifndef STREAM_MANAGER_H
#define STREAM_MANAGER_H

#include <cstdint>
#include <list>
#include <mutex>
#include "StreamClient.h"
#include "types.h"

class Manager {
public:
  Manager();
  ~Manager();
  // ********************
  std::mutex& referMutex();
  // ********************
  ClientToken addClient();
  int removeClient(const ClientToken t);
  // ********************
  AudioVector getOtherClientStreams(const ClientToken t);
  ClientTID getClientResponseTID(const ClientToken t);
  int updateClientStream(const ClientToken t, const ClientTID tid, const AudioVector& v);
private:
  typedef std::list<StreamClient> ClientList;
  // ********************
  void debugPrint();
  // ********************
  ClientList myClients;
  std::mutex myMutex;
  ClientToken myLastToken;
};

#endif
