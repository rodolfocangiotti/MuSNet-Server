#include <cassert>
#include <iostream>
#include "Manager.h"
#include "commons.h"
#include "prettyprint.h"

Manager::Manager():
  myClients(),
  myMutex(),
  myTokenHist(0) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing Manager class..." << '\n';
#endif
}

Manager::~Manager() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing Manager class..." << '\n';
#endif
}

ClientToken Manager::addClient() {
  std::lock_guard<std::mutex> l(myMutex);
  ClientToken t = ++myTokenHist;
  /*
  myClients.emplace_back(StreamClient(t));
  for (int i = 0; i < myClients.size() - 1; i++) {
    myClients[i].addReader(t);
    ClientToken u = myClients[i].token();
    myClients[myClients.size() - 1].addReader(u);
  }
  */
  StreamClient nsc = StreamClient(t);
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    sc->addReader(t);
    ClientToken ot = sc->token();
    nsc.addReader(ot);
  }
  myClients.push_back(nsc);
#if defined(DEBUG) && VERBOSENESS > 0
  std::cout << GREEN << "[DEBUG] Added new client with token: " << t << RESET << '\n';
  debugPrint();
#endif
  return t; // TODO Return also -1 if errors...
}

int Manager::removeClient(ClientToken t) {
  std::lock_guard<std::mutex> l(myMutex);
  bool found = false;
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {  // Check if there is a client connected with the argument token...
    if (sc->token() == t) {
      found = true;
      myClients.erase(sc);
#if defined(DEBUG) && VERBOSENESS > 0
      std::cout << GREEN << "[DEBUG] Removed client with token: " << t << RESET << '\n';
      debugPrint();
#endif
      break;
    }
  }
  if (!(found)) {
    std::cerr << RED << "[ERROR] Impossible to remove client (token not found)!" << RESET << '\n';
    return -1;
  }
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {  // If client has successfully been removed, remove read permissions from other clients' register...
    sc->removeReader(t);
  }
  return 0;
}

AudioVector Manager::getOtherClientStreams(ClientToken t) {
  // DONE
  std::lock_guard<std::mutex> l(myMutex);
  AudioVector v(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0);
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    if (sc->token() == t) {
      continue; // Skip current client stream...
    }
    AudioVector o = sc->retrieveVector(t);
    for (int j = 0; j < v.size(); j++) {
      v[j] += o[j];
    }
  }
  return v;
}

ClientTID Manager::getClientResponseTID(ClientToken t) {
  // DONE
  std::lock_guard<std::mutex> l(myMutex);
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    if (sc->token() == t) {
      return sc->getNewResponseTID();
    }
  }
  std::cerr << RED << "[ERROR] Impossible to get client response TID (token not found)!" << RESET << '\n';
  return -1;
}

int Manager::updateClientStream(ClientToken t, ClientTID tid, AudioVector& v) {
  // DONE
  std::lock_guard<std::mutex> l(myMutex);
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    if (sc->token() == t) {
      sc->insertVector(t, tid, v);
      return 0;
    }
  }
  std::cerr << RED << "[ERROR] Impossible to update client stream (token not found)!" << RESET << '\n';
  return -1;
}

void Manager::debugPrint() {
  std::cout << CYAN << myClients.size() << " connected!" << RESET << '\n';
  int i = 1;
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    std::cout << CYAN << "Client n." << i++ << " uses token " << sc->token() << RESET << '\n';
  }
}
