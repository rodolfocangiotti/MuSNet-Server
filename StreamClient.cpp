#include <cassert>
#include <iostream>
#include "StreamClient.h"

StreamClient::StreamClient(const Token t):
  myAudioVector(512 * 2), // TODO...
  myID(0),
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
  return myAudioVector;
}

StreamClient::ID StreamClient::id() const {
  return myID;
}

StreamClient::Token StreamClient::token() const {
  return myToken;
}

void* StreamClient::pointWritableVector() {
  return myAudioVector.data();
}

void StreamClient::updateID(ID i) {
  assert(i > myID);
  myID = i;
}

void StreamClient::clearVector() {
  for (int i = 0; i < myAudioVector.size(); i++) {
    myAudioVector[i] = 0.0;
  }
}

void StreamClient::updateVector(AudioVector v) {
  for (int i = 0; i < myAudioVector.size(); i++) {
    myAudioVector[i] = v[i];
  }
}
