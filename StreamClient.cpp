#include <cassert>
#include <iostream>
#include "StreamClient.h"
#include "commons.h"

StreamClient::StreamClient(const Token t):
  myAudioVect(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0),
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

void StreamClient::updateTID(TID tid) {
  if (!(tid > myTID)) {
    std::cout << "Client token: " << myToken << std::endl;
    std::cout << "Last TID: " << myTID << "\tCurrent TID: " << tid << std::endl;
  }
  assert(tid > myTID);
  myTID = tid;
}

void StreamClient::clearVector() {
  for (int i = 0; i < myAudioVect.size(); i++) {
    myAudioVect[i] = 0.0;
  }
}

void StreamClient::updateVector(AudioVector v) {
  assert(v.size() == myAudioVect.size());
  for (int i = 0; i < myAudioVect.size(); i++) {
    myAudioVect[i] = v[i];
  }
}
