#include <cassert>
#include <iostream>
#include "StreamVector.h"
#include "commons.h"
#include "prettyprint.h"
#include "utils.h"

StreamVector::StreamVector():
  myVect(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0),
  myLog(),
  myOwner(0), myTID(0) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Constructing StreamVector class..." << '\n';
#endif
}

StreamVector::~StreamVector() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Destructing StreamVector class..." << '\n';
#endif
}

ClientToken StreamVector::owner() const {
  return myOwner;
}

ClientTID StreamVector::tid() const {
  return myTID;
}

void StreamVector::setAudioVector(const AudioVector& v) {
  myVect = v;
}

void StreamVector::setOwner(const ClientToken t) {
  myOwner = t;
}

void StreamVector::setTID(const ClientTID tid) {
  myTID = tid;
}

int StreamVector::addReadPermission(const ClientToken t) {
  assert(t != myOwner);
  assert(myLog.count(t) == 0);
  myLog[t] = false;
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Vector " << myOwner << "-" << myTID << ": added read permission to client " << t << '\n';
#endif
  return 0;
}

int StreamVector::removeReadPermission(const ClientToken t) {
  assert(t != myOwner);
  assert(myLog.count(t) == 1);
  for (ReadLogger::iterator i = myLog.begin(); i != myLog.end(); i++) {
    if (i->first == t) {
      myLog.erase(i);
  #if defined(DEBUG) && VERBOSENESS > 2
      std::cout << getUTCTime() << " [DEBUG] Vector " << myOwner << "-" << myTID << ": removed read permission to client " << t << '\n';
  #endif
      return 0;
    }
  }
  std::cerr << getUTCTime() << RED << " [ERROR] Vector " << myOwner << "-" << myTID << ": impossible to remove read permission to client " << t << " (not found)!" << RESET << '\n';
  return -1;
}

AudioVector StreamVector::readVector(const ClientToken t) {
  assert(myLog.count(t) == 1); // If not, client has no read permission...
  assert(myLog[t] == false);  // Client has not still read the vector...
  myLog[t] = true;
  return myVect;
}

bool StreamVector::isDeletable() {
  for (ReadLogger::iterator i = myLog.begin(); i != myLog.end(); i++) {
    if (i->second == false) {
      return false;
    }
  }
  return true;
}

bool StreamVector::isReadableBy(const ClientToken t) {
  if ((myLog.count(t) > 0) && (myLog[t] == false)) {
    return true;
  }
  return false;
}
