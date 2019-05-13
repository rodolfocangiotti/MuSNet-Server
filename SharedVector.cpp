#include <cassert>
#include <iostream>
#include "SharedVector.h"
#include "commons.h"
#include "prettyprint.h"

SharedVector::SharedVector():
  myOwner(0),
  myVID(0),
  cleared(true),
  myLog(),
  myVect(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing SharedVector class..." << '\n';
#endif
}

SharedVector::~SharedVector() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing SharedVector class..." << '\n';
#endif
}

void SharedVector::setOwner(ClientToken t) {
  myOwner = t;
}

void SharedVector::setVID(ClientVID vid) {
  myVID = vid;
}

int SharedVector::addReader(ClientToken t) {
  myLog[t] = 0;
#if defined(DEBUG) && VERBOSENESS > 0
  std::cout << "[DEBUG] Adding reader " << t << " for vector " << myVID << " of client " << myOwner << "..." << '\n';
#endif
  return 0; // TODO Add error management, returning -1...
}

int SharedVector::removeReader(ClientToken t) {
  for (ReadLogger::iterator i = myLog.begin(); i != myLog.end(); i++) {
    if (i->first == t) {
      myLog.erase(i);
#if defined(DEBUG) && VERBOSENESS > 0
      std::cout << "[DEBUG] Removing reader " << t << " from vector " << myVID << " of client " << myOwner << "..." << '\n';
#endif
      return 0;
    }
  }
  std::cerr << RED << "[ERROR] Impossible to remove reader (token " << t << " not found)!" << RESET << '\n';
  return -1;
}

void SharedVector::clear() {
  //assert(!(cleared)); ??????????
  for (int i = 0; i < myVect.size(); i++) {
    myVect[i] = 0.0;
  }
  cleared = true;
}

AudioVector SharedVector::read(ClientToken t) {
  //assert(!(cleared)); ??????????
  myLog[t] += 1;
#if defined(DEBUG) && VERBOSENESS > 1
  if (myLog[t] > 1) {
    std::cout << YELLOW << "[DEBUG] Client " << t << " has read vector " << myOwner << "-" << myVID << " " << static_cast<int>(myLog[t]) << " times..." << RESET << '\n';
  }
#endif
  return myVect;
}

void SharedVector::write(AudioVector& v) {
  assert(v.size() == myVect.size());
  /*
  if (myLog.size() > 0) {
    if (!(cleared))
      std::cout << "Client: " << myOwner << " -- Vector: " << myVID << '\n';
    assert(cleared);
  } THIS ASSERTION OFTEN RAISES ERROR. MAYBE WHEN NEW CLIENT IS ADDEDD, THE UPDATE (WRITE) OPERATION CAN OCCUR RIGHT AFTER, BEFORE THE VECTOR IS ACTUALLY READ BY ALL THE CLIENTS...
  */
  for (int i = 0; i < myVect.size(); i++) { // TODO Check if this iteration can be avoided...
    myVect[i] = v[i];
  }
  cleared = false;
}

bool SharedVector::isToClear() {
  //assert(!(cleared)); ??????????
  for (ReadLogger::iterator i = myLog.begin(); i != myLog.end(); i++) {
    if (i->second == 0) {
      return false;
    }
  }
#if defined(DEBUG) && VERBOSENESS > 0 // TODO Remove it after tests!
  //printReadStatus();
#endif
  for (ReadLogger::iterator i = myLog.begin(); i != myLog.end(); i++) {
    i->second = 0;  // ?????
  }
  return true;
}

void SharedVector::printReadStatus() {
  // This debug function is called before a vector is cleared!
  std::cout << CYAN << "Vector " << myVID << " of client " << myOwner << " has been read: " << '\n';
  for (ReadLogger::iterator i = myLog.begin(); i != myLog.end(); i++) {
    std::cout << static_cast<int>(i->second) << " times from client " << i->first << '\t';
  }
  std::cout << RESET << '\n';
}
