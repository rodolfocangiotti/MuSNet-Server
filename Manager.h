#ifndef STREAM_MANAGER_H
#define STREAM_MANAGER_H

#include <cstdint>
#include <list>
#include <mutex>
#include <thread>
//#include <vector>
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
  typedef std::list<StreamClient> ClientList;
  // ********************
  void debugPrint();
  // ********************
  ClientList myClients;
  std::mutex myMutex;
  ClientToken myTokenHist;
  //std::vector<StreamClient> myClients;
};

#endif
