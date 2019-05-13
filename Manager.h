#ifndef STREAM_MANAGER_H
#define STREAM_MANAGER_H

#include <cstdint>
#include <mutex>
#include <thread>
#include <vector>
#include "StreamClient.h"
#include "types.h"

class Manager {
public:
  Manager();
  ~Manager();
  // ********************
  ClientToken addClient();
  int removeClient(ClientToken t);
  // ********************
  AudioVector getOtherClientStreams(ClientToken t);
  ClientTID getClientResponseTID(ClientToken t);
  int updateClientStream(ClientToken t, ClientTID tid, AudioVector& v);
private:
  void debugPrint();
  ClientToken myTokenHist;
  std::mutex myMutex;
  std::vector<StreamClient> myClients;
};

#endif
