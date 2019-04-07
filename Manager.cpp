#include <cassert>
#include <iostream>
#include "Manager.h"
#include "commons.h"

Manager::Manager():
  myClients(),
  myTokenHist(0) {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing Manager class..." << std::endl;
#endif
}

Manager::~Manager() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing Manager class..." << std::endl;
#endif
}

StreamClient::Token Manager::addClient() {
  std::lock_guard<std::mutex> l(myMutex);
  StreamClient::Token t = ++myTokenHist;
  myClients.emplace_back(StreamClient(t));
#ifdef DEBUG
  std::cout << "[DEBUG] Added new client with token: " << t << std::endl;
  debugPrint();
#endif
  return t;
}

void Manager::removeClient(StreamClient::Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients.erase(myClients.begin() + i);
      break;
    }
  }
#ifdef DEBUG
  std::cout << "[DEBUG] Removed client with token: " << t << std::endl;
  debugPrint();
#endif
}

AudioVector Manager::getOtherClientStreams(StreamClient::Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  AudioVector v(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      continue;
    }
    const AudioVector o = myClients[i].audioVector();
    assert(v.size() == o.size());
    for (int j = 0; j < v.size(); j++) {
      v[j] += o[j];
    }
  }
  return v;
}

StreamClient::TID Manager::getClientTID(StreamClient::Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      return myClients[i].tid();
    }
  }
  return 0;
}

void Manager::updateClientTID(StreamClient::Token t, StreamClient::TID n) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int j = 0; j < myClients.size(); j++) {
    if (myClients[j].token() == t) {
      myClients[j].updateTID(n);
      break;
    }
  }
}

void Manager::clearClientStream(StreamClient::Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients[i].clearVector();
      break;
    }
  }
}

void Manager::updateClientStream(StreamClient::Token t, AudioVector v) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients[i].updateVector(v);
      break;
    }
  }
}

void Manager::debugPrint() {
  std::cout << myClients.size() << " connected!" << std::endl;
  for (int i = 0; i < myClients.size(); i++) {
    std::cout << "Client " << i << " uses token " << myClients[i].token() << std::endl;
  }
}
