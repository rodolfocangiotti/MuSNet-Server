#include <cassert>
#include <chrono>
#include <iostream>
#include "Console.h"
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
  Console::log(getUTCTime() + " [DEBUG] Constructing Manager class...");
#endif
}

Manager::~Manager() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Destructing Manager class...");
#endif
}

std::mutex& Manager::referMutex() {
  return myMutex;
}

ClientToken Manager::addClient() {
  std::lock_guard<std::mutex> l(myMutex);
  ClientToken t = ++myLastToken;
  StreamClient nsc = StreamClient(t);
  // for (ClientList::iterator osc = myClients.begin(); osc != myClients.end(); osc++) {
  //   osc->addReader(t);
  //   ClientToken ot = osc->token();
  //   nsc.addReader(ot);
  // }
  /*
  Sospetto che il blocco sopra commentato ritardi lo streaming del tempo del ping più ritardi addizionali dovuti al TCP.
  Sposto l'operazione in updateClientStream()
  */
  myClients.push_back(nsc);
#if defined(DEBUG) && VERBOSENESS > 0
  Console::log(getUTCTime() + " [DEBUG] Added new client with token: " + str(t), Color::Green);
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
      Console::log(getUTCTime() + " [DEBUG] Removed client with token: " + str(t), Color::Green);
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
  // TODO: To remove this.
  /*
  bool empty = true;
  for (int i {0}; i < v.size(); i++) {
    if (v[i] != 0.0) {
      empty = false;
      break;
    }
  }
  if (empty) {
    std::cout << "getOtherClientStreams: returning empty vector for token " << t << '\n';
  }
  */
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
  StreamClient* _sc = nullptr;
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    if (sc->token() == t) {
      // sc->insertVector(t, tid, v);
      // return 0;
      _sc = &(*sc);
      break;
    }
  }
  if (_sc == nullptr) {
    // XXX ERRORE GRAVE...
    return -1;
  }
  if (_sc->isWaiting()) {
    // std::cout << RED << "ENTERING IN THE INFERNO!" << RESET << '\n';
    for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
      if (sc->token() != t) {
        sc->addReader(t);
        ClientToken ot = sc->token();
        _sc->addReader(ot);
        std::cout << YELLOW << "ADDING READ PERMISSIONS TO " << t << " ON CLIENT " << ot << RESET << '\n';
        std::cout << CYAN << "ADDING READ PERMISSIONS TO " << ot << " ON CLIENT " << t << RESET << '\n';
      }
    }
  }
  // std::cout << RED << "INSERTING VECTOR!" << RESET << '\n';
  _sc->insertVector(t, tid, v);
  return 0;
  // std::cerr << getUTCTime() << RED << " [ERROR] Impossible to update client stream (token not found)!" << RESET << '\n';
  // return -1;
}

void Manager::debugPrint() {
  Console::log(getUTCTime() + str(myClients.size()) + " connected!", Color::Cyan);
  int i = 1;
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    Console::log(getUTCTime() + " Client n." + str(i++) + " uses token " + str(sc->token()), Color::Cyan);
  }
}

const std::list<ClientTID> Manager::getTIDHistory(ClientToken tk) {
  for (ClientList::iterator sc = myClients.begin(); sc != myClients.end(); sc++) {
    if (sc->token() == tk) {
      return sc->tidHistory();
    }
  }
  std::cerr << "Argh! No history found\n"; // TODO
  return std::list<ClientTID>();
}

bool Manager::is_valid_token(ClientToken token) {
    for (ClientList::iterator it = myClients.begin(); it != myClients.end(); it++) {
        if (it->token() == token) {
            return true;
        }
    }
    return false;
}
