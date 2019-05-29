#include <cassert>
#include <chrono>
#include <iostream>
#include "Manager.h"
#include "commons.h"
#include "prettyprint.h"
#include "utils.h"

// TODO Improve code about memory usage (for instance, using std::move())!

Manager::Manager():
  myClients(),
  myMutex(),
  myLastToken(0) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Destructing Manager class..." << '\n';
#endif
}

Manager::~Manager() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Destructing Manager class..." << '\n';
#endif
}

std::mutex& Manager::referMutex() {
  return myMutex;
}

ClientToken Manager::addClient() {
  std::lock_guard<std::mutex> l(myMutex);
  ClientToken t = ++myLastToken;
  StreamClient nsc = StreamClient(t);
  for (ClientList::iterator osc = myClients.begin(); osc != myClients.end(); osc++) {
    osc->addReader(t);
    ClientToken ot = osc->token();
    nsc.addReader(ot);
  }
  myClients.push_back(nsc);
#if defined(DEBUG) && VERBOSENESS > 0
  std::cout << getUTCTime() << GREEN << " [DEBUG] Added new client with token: " << t << RESET << '\n';
  debugPrint();
#endif
  return t; // TODO Check if errors can occur! If yes, return -1!
}

int Manager::removeClient(const ClientToken t) {
  std::lock_guard<std::mutex> l(myMutex);
  bool found = false;
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {  // Check if there is a client connected with the argument token...
    if (sc->token() == t) {
      found = true;
      myClients.erase(sc);
#if defined(DEBUG) && VERBOSENESS > 0
      std::cout << getUTCTime() << GREEN << " [DEBUG] Removed client with token: " << t << RESET << '\n';
      debugPrint();
#endif
      break;
    }
  }
  if (!(found)) {
    std::cerr << getUTCTime() << RED << " [ERROR] Impossible to remove client (token not found)!" << RESET << '\n';
    return -1;
  }
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {  // If client has successfully been removed, remove read permissions from other clients' register...
    sc->removeReader(t);
  }
  return 0;
}

AudioVector Manager::getOtherClientStreams(const ClientToken t) {
  assert(!(myMutex.try_lock()));  // XXX MUTEX SHOULD BE ALREADY LOCKED!
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

ClientTID Manager::getClientResponseTID(const ClientToken t) {
  assert(!(myMutex.try_lock()));  // XXX MUTEX SHOULD BE ALREADY LOCKED!
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    if (sc->token() == t) {
      return sc->getNewResponseTID();
    }
  }
  std::cerr << getUTCTime() << RED << " [ERROR] Impossible to get client response TID (token not found)!" << RESET << '\n';
  return -1;
}

int Manager::updateClientStream(const ClientToken t, const ClientTID tid, const AudioVector& v) {
  assert(!(myMutex.try_lock())); // XXX MUTEX SHOULD BE ALREADY LOCKED!
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    if (sc->token() == t) {
      sc->insertVector(t, tid, v);
      return 0;
    }
  }
  std::cerr << getUTCTime() << RED << " [ERROR] Impossible to update client stream (token not found)!" << RESET << '\n';
  return -1;
}

void Manager::debugPrint() {
  std::cout << getUTCTime() << CYAN << myClients.size() << " connected!" << RESET << '\n';
  int i = 1;
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    std::cout << getUTCTime() << CYAN << " Client n." << i++ << " uses token " << sc->token() << RESET << '\n';
  }
}
