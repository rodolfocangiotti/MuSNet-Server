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
  Token addClient();
  void removeClient(Token t);
  AudioVector getOtherStreams(Token t);
private:
  std::mutex myMutex;
  std::vector<StreamClient> myClients;
};

#endif
