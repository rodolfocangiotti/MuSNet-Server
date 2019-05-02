#include <cassert>
#include <iostream>
#include "Manager.h"
#include "commons.h"
#include "prettyprint.h"

Manager::Manager():
  myClients(),
  myTokenHist(0) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing Manager class..." << std::endl;
#endif
}

Manager::~Manager() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing Manager class..." << std::endl;
#endif
}

StreamClient::Token Manager::addClient() {
  std::lock_guard<std::mutex> l(myMutex);
  StreamClient::Token t = ++myTokenHist;
  myClients.emplace_back(StreamClient(t));
  for (int i = 0; i < myClients.size() - 1; i++) {
    myClients[i].addReader(t);
    StreamClient::Token u = myClients[i].token();
    myClients[myClients.size() - 1].addReader(u);
  }
#if defined(DEBUG) && VERBOSENESS > 0
  std::cout << GREEN << "[DEBUG] Added new client with token: " << t << RESET << std::endl;
  debugPrint();
#endif
  return t; // TODO Return also -1 if errors...
}

int Manager::removeClient(StreamClient::Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  bool found = false;
  for (int i = 0; i < myClients.size(); i++) {  // Check if there is a client connected with the argument token...
    if (myClients[i].token() == t) {
      found = true;
      myClients.erase(myClients.begin() + i);
#if defined(DEBUG) && VERBOSENESS > 0
      std::cout << GREEN << "[DEBUG] Removed client with token: " << t << RESET << std::endl;
      debugPrint();
#endif
      break;
    }
  }
  if (!(found)) {
    std::cerr << RED << "[ERROR] Impossible to remove client (token not found)!" << RESET << std::endl;
    return -1;
  }
  for (int i = 0; i < myClients.size(); i++) {  // If client has successfully been removed, remove read permissions from other clients' register...
    myClients[i].removeReader(t);
  }
  return 0;
}

AudioVector Manager::getOtherClientStreams(StreamClient::Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  AudioVector v(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0);
  assert(v.capacity() == v.size());
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      continue; // Skip current client stream...
    }
    AudioVector o = myClients[i].readVector(t);
    assert(o.capacity() == o.size());
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
  std::cerr << RED << "[ERROR] Impossible to get client TID (token not found)!" << RESET << std::endl;
  return -1;
}

int Manager::updateClientTID(StreamClient::Token t, StreamClient::TID n) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int j = 0; j < myClients.size(); j++) {
    if (myClients[j].token() == t) {
      myClients[j].updateTID(n);
      return 0;
    }
  }
  std::cerr << RED << "[ERROR] Impossible to update client TID (token not found)!" << RESET << std::endl;
  return -1;
}

int Manager::clearClientStream(StreamClient::Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients[i].clearVector();
      return 0;
    }
  }
  std::cerr << RED << "[ERROR] Impossible to clear client stream (token not found)!" << RESET << std::endl;
  return -1;
}

int Manager::updateClientStream(StreamClient::Token t, AudioVector& v) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients[i].writeVector(v);
      return 0;
    }
  }
  std::cerr << RED << "[ERROR] Impossible to update client stream (token not found)!" << RESET << std::endl;
  return -1;
}

void Manager::debugPrint() {
  std::cout << CYAN << myClients.size() << " connected!" << RESET << std::endl;
  for (int i = 0; i < myClients.size(); i++) {
    std::cout << CYAN << "Client " << i << " uses token " << myClients[i].token() << RESET << std::endl;
  }
}
