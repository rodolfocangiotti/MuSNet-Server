#include <cassert>
#include <iostream>
#include "StreamClient.h"
#include "commons.h"
#include "prettyprint.h"
#include "utils.h"

bool comp(const StreamVector& a, const StreamVector& b) {
  return a.tid() < b.tid();
}

StreamClient::StreamClient(const ClientToken t):
  myQueue(),
  myReaders(),
  myLastResp(0),
  myToken(t),
  myQueueMaxSize(0) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Constructing StreamClient class..." << '\n';
#endif
}

StreamClient::~StreamClient() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Destructing StreamClient class..." << '\n';
#endif
}

ClientToken StreamClient::token() const {
  return myToken;
}


ClientTID StreamClient::getNewResponseTID() {
  return ++myLastResp;
}

int StreamClient::addReader(const ClientToken ot) {
  myReaders.push_back(ot);  // Push token in the readers' list...
#if defined(DEBUG) && VERBOSENESS > 1
  std::cout << getUTCTime() << " [DEBUG] Client " << myToken << ": added reader " << ot << '\n';
#endif
  return 0; // TODO Check if errors can occur! If yes, return -1!
}

int StreamClient::removeReader(const ClientToken ot) {
  bool found = false;
  for (ReadManager::iterator t = myReaders.begin(); t != myReaders.end(); t++) {
    if (*t == ot) {
      myReaders.erase(t);
      found = true;
      break;
    }
  }
  if (!(found)) {
    std::cerr << getUTCTime() << RED << " [ERROR] Client " << myToken << ": impossible to remove reader " << ot << " (not found)!" << RESET << '\n';
    return -1;
  }
  for (StreamQueue::iterator sv = myQueue.begin(); sv != myQueue.end(); sv++) { // If reader has been found, remove read permission from all the vectors on queue...
    sv->removeReadPermission(ot);
  }
#if defined(DEBUG) && VERBOSENESS > 1
  std::cout << getUTCTime() << " [DEBUG] Client " << myToken << ": removed reader " << ot << '\n';
#endif
  return 0;
}

void StreamClient::insertVector(const ClientToken mt, const ClientTID wtid, const AudioVector& v) {
  assert(mt == myToken);
  if (myReaders.size() > 0) {
    StreamVector sv;
    sv.setOwner(mt);
    sv.setTID(wtid);
    sv.setAudioVector(v);
    for (ReadManager::iterator t = myReaders.begin(); t != myReaders.end(); t++) {
      sv.addReadPermission(*t);
    }
    myQueue.push_back(sv);
    myQueue.sort(comp);

    myQueueMaxSize = myQueue.size() > myQueueMaxSize ? myQueue.size() : myQueueMaxSize;
#if defined(DEBUG) && VERBOSENESS > 1
    std::cout << getUTCTime() << GREEN << " [DEBUG] Client " << myToken << ": inserted new vector on queue (TID: " << wtid << ")" << RESET << '\n';
    std::cout << YELLOW << "Current queue size: " << myQueue.size() << '\n';
    std::cout << "Maximum queue size reached: " << myQueueMaxSize << RESET << '\n';
#endif
  } else {
    if (myQueue.size() > 0) {
      myQueue.clear();  // Reset latency if present...
    }
  }
}

AudioVector StreamClient::retrieveVector(const ClientToken ot) {
  AudioVector v(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0);
  bool found = false;
  for (StreamQueue::iterator sv = myQueue.begin(); sv != myQueue.end(); sv++) {
    if (sv->isReadableBy(ot)) {
      found = true;
      v = sv->readVector(ot);
      if (sv->isDeletable()) {
        myQueue.erase(sv);
#if defined(DEBUG) && VERBOSENESS > 1
        std::cout << getUTCTime() << MAGENTA << " [DEBUG] Client " << myToken << ": removed vector from queue" << RESET << '\n';
#endif
      }
      break;
    }
  }
  if (!(found)) {
    std::cerr << getUTCTime() << RED << " [ERROR] Client " << myToken << ": no readable vector found (requested by client " << ot << ")" << RESET << '\n';
  }
  return v;
}
