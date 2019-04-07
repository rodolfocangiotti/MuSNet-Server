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
  void removeClient(StreamClient::Token t);
  void updateClientTID(StreamClient::Token t, StreamClient::TID i);
  void updateClientStream(StreamClient::Token t, AudioVector v);
  void clearClientStream(StreamClient::Token t);
private:
  void debugPrint();
  StreamClient::Token myTokenHist;
  std::mutex myMutex;
  std::vector<StreamClient> myClients;
};

#endif
