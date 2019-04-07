#include <cassert>
#include <iostream>
#include "StreamClient.h"
#include "commons.h"

StreamClient::StreamClient(const Token t):
  myAudioVect(AUDIO_VECTOR_SIZE * NUM_CHANNELS),
  myTID(0),
  myToken(t) {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing StreamClient class..." << std::endl;
#endif
}

StreamClient::~StreamClient() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing StreamClient class..." << std::endl;
#endif
}

const AudioVector& StreamClient::audioVector() const {
  return myAudioVect;
}

StreamClient::TID StreamClient::tid() const {
  return myTID;
}

StreamClient::Token StreamClient::token() const {
  return myToken;
}

void* StreamClient::pointWritableVector() {
  return myAudioVect.data();
}

void StreamClient::updateTID(TID n) {
  if (!(n > myTID)) {
    std::cout << "Client token: " << myToken << std::endl;
    std::cout << "Last TID: " << myTID << "\tCurrent TID: " << n << std::endl;
  }
  assert(n > myTID);
  myTID = n;
}

void StreamClient::clearVector() {
  for (int i = 0; i < myAudioVect.size(); i++) {
    myAudioVect[i] = 0.0;
  }
}

void StreamClient::updateVector(AudioVector v) {
  assert(myAudioVect.size() == v.size());
  for (int i = 0; i < myAudioVect.size(); i++) {
    myAudioVect[i] = v[i];
  }
}
