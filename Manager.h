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
  typedef uint32_t Token;
  Manager();
  ~Manager();
  // ********************
  AudioVector getOtherClientStreams(Token t);
  StreamClient::ID getClientID(Token t);
  // ********************
  Token addClient();
  void removeClient(Token t);
  void updateClientID(Token t, StreamClient::ID i);
  void updateClientStream(Token t, AudioVector v);
  void clearClientStream(Token t);
private:
  std::mutex myMutex;
  std::vector<StreamClient> myClients;
};

#endif
