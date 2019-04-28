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
  AudioVector getOtherClientStreams(StreamClient::Token t);
  StreamClient::TID getClientTID(StreamClient::Token t);
  // ********************
  StreamClient::Token addClient();
  int removeClient(StreamClient::Token t);
  int updateClientTID(StreamClient::Token t, StreamClient::TID i);
  int updateClientStream(StreamClient::Token t, AudioVector& v);
  int clearClientStream(StreamClient::Token t);
private:
  void debugPrint();
  StreamClient::Token myTokenHist;
  std::mutex myMutex;
  std::vector<StreamClient> myClients;
};

#endif
