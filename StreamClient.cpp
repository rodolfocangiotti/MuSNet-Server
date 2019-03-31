#include <cassert>
#include <iostream>
#include "StreamClient.h"

StreamClient::StreamClient(const Token t):
  myLastID(0), myBuffer(9000), myToken(t) { // TODO
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing StreamClient class..." << std::endl;
#endif
}

StreamClient::~StreamClient() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing StreamClient class..." << std::endl;
#endif
}

const SharedBuffer& StreamClient::sharedBuffer() const {
  return myBuffer;
}

StreamClient::Token StreamClient::token() const {
  return myToken;
}

StreamClient::ID StreamClient::lastID() const {
  return myLastID;
}

void StreamClient::updateID(ID i) {
  assert(i > myLastID);
  myLastID = i;
}

SharedBuffer* StreamClient::pointSharedBuffer() {
  return &myBuffer;
}
