#include <cassert>
#include <iostream>
#include "StreamClient.h"
#include "commons.h"
#include "prettyprint.h"

StreamClient::StreamClient(const Token t):
  myBuff(AUDIO_BUFFER_SIZE * NUM_CHANNELS, 0.0),
  myIndx(0),
  myTID(0),
  myToken(t) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing StreamClient class..." << std::endl;
#endif
}

StreamClient::~StreamClient() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing StreamClient class..." << std::endl;
#endif
}

StreamClient::TID StreamClient::tid() const {
  return myTID;
}

StreamClient::Token StreamClient::token() const {
  return myToken;
}

void StreamClient::clearVector() {
  Index j = myIndx;
  for (int i = 0; i < AUDIO_VECTOR_SIZE * NUM_CHANNELS; i++) {
    myBuff[j] = 0.0;
    j++;
    j = j < myBuff.size() ? j : j - myBuff.size();
  }
  myIndx = j;
}

AudioVector StreamClient::readVector(Token t) {
  AudioVector v(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0);
  assert(v.capacity() == v.size());
  if (!(myReaders.count(t) > 0)) {
    std::cerr << RED << "[ERROR] Not found read permission for client " << t << "for the buffer of client " << myToken << "!" << RESET << std::endl;
    return v;
  }
  Index j = myReaders[t];
  for (int i = 0; i < v.size(); i++) {
    v[i] = myBuff[j];
    j++;
    j = j < myBuff.size() ? j : j - myBuff.size();
  }
  myReaders[t] = j; // Update index history...
  return v;
}

void StreamClient::writeVector(AudioVector& v) {
  assert(v.size() == AUDIO_VECTOR_SIZE * NUM_CHANNELS);
  Index j = myIndx;
  for (int i = 0; i < v.size(); i++) {
    myBuff[j] = v[i];
    j++;
    j = j < myBuff.size() ? j : j - myBuff.size();
  }
  myIndx = j;
}

void StreamClient::updateTID(TID tid) {
  if (!(tid > myTID)) {
    std::cerr << YELLOW << "Client token: " << myToken << std::endl;
    std::cerr << "Last TID: " << myTID << "\tCurrent TID: " << tid << RESET << std::endl;
  }
  assert(tid > myTID);
  myTID = tid;
}

int StreamClient::addReader(Token t) {
  Index i = myIndx - AUDIO_VECTOR_SIZE * NUM_CHANNELS;
  myReaders[t] = i < myBuff.size() ? i : i + myBuff.size();
#if defined(DEBUG) && VERBOSENESS > 0
  std::cout << "[DEBUG] Adding reader " << t << " for client " << myToken << "..." << std::endl;
#endif
  return 0; // TODO Add error management, returning -1...
}

int StreamClient::removeReader(Token t) {
  for (AccessManager::iterator i = myReaders.begin(); i != myReaders.end(); i++) {
    if (i->first == t) {
      myReaders.erase(i);
#if defined(DEBUG) && VERBOSENESS > 0
      std::cout << "[DEBUG] Removing reader " << t << " for client " << myToken << "..." << std::endl;
#endif
      return 0;
    }
  }
  std::cerr << RED << "[ERROR] Impossible to remove reader (token not found)!" << RESET << std::endl;
  return -1;
}
