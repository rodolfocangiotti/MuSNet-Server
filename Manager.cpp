#include <cassert>
#include <iostream>
#include "Manager.h"

Manager::Manager():
  myClients() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing Manager class..." << std::endl;
#endif
}

Manager::~Manager() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing Manager class..." << std::endl;
#endif
}

Manager::Token Manager::addClient() {
  std::cout << "ADDING CLIENT..." << std::endl;
  static Token t = 0;
  {
    std::lock_guard<std::mutex> l(myMutex);
    myClients.emplace_back(StreamClient(t++));
  }
  return t;
}

void Manager::removeClient(Token t) {
  std::cout << "REMOVING CLIENT..." << std::endl;
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients.erase(myClients.begin() + i);
      break;
    }
  }
}

AudioVector Manager::getOtherClientStreams(Token t) {
  AudioVector v(512 * 2, 0.0);  // TODO
  {
    std::lock_guard<std::mutex> l(myMutex);
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
  }
  return v;
}

StreamClient::ID Manager::getClientID(Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      return myClients[i].id();
    }
  }
  return -1;
}

void Manager::updateClientID(Token t, StreamClient::ID i) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int j = 0; j < myClients.size(); j++) {
    if (myClients[j].token() == t) {
      myClients[j].updateID(i);
      break;
    }
  }
}

void Manager::clearClientStream(Token t) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients[i].clearVector();
      break;
    }
  }
}

void Manager::updateClientStream(Token t, AudioVector v) {
  std::lock_guard<std::mutex> l(myMutex);
  for (int i = 0; i < myClients.size(); i++) {
    if (myClients[i].token() == t) {
      myClients[i].updateVector(v);
      break;
    }
  }
}
