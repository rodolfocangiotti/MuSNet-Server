#include <unistd.h>
#include <cassert>
#include <iostream>
#include "SharedBuffer.h"

SharedBuffer::SharedBuffer(Size s):
  //v(s), m() {
  v(s) {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing SharedBuffer class..." << std::endl;
#endif
}

SharedBuffer::~SharedBuffer() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing SharedBuffer class..." << std::endl;
#endif
}

AudioSample SharedBuffer::operator[](Index i) {
  //std::lock_guard<std::mutex> l(m);
  return v[i];
}

AudioSample SharedBuffer::at(Index i) {
  //std::lock_guard<std::mutex> l(m);
  return v[i];
}

AudioVector SharedBuffer::copy() const {
  AudioVector c(v.begin(), v.end());
  return c;
}

SharedBuffer::Size SharedBuffer::size() const {
  return v.size();
}

void SharedBuffer::update(const AudioVector& o) {
  //std::lock_guard<std::mutex> l(m);
  for (int i = 0; i < v.size(); i++) {
    v[i] = o[i];
  }
}

void SharedBuffer::clear() {
  //std::lock_guard<std::mutex> l(m);
  for (int i = 0; i < v.size(); i++) {
    v[i] = 0.0;
  }
}
