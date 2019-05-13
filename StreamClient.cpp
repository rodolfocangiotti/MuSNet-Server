#include <cassert>
#include <iostream>
#include "StreamClient.h"
#include "commons.h"
#include "prettyprint.h"

bool comp(const StreamVector& a, const StreamVector& b) {
  return a.tid() < b.tid();
}

StreamClient::StreamClient(const ClientToken t):
  myQueue(),
  myLastResp(0), //myLastWReq(0),
  myToken(t), myReaders() {
  //myRReqsts(), myWReqsts(1, 0) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing StreamClient class..." << '\n';
#endif
  /*
  for (int i = 0; i < myBuff.size(); i++) {
    myBuff[i].setOwner(myToken);
    myBuff[i].setVID(i);
  }
  myWReqsts.reserve(AUDIO_VECTOR_SIZE);
  myWReqsts.push_back(0); // Initialize vector with a default value...

  std::cout << "myWReqsts.size() at constructor: " << myWReqsts.size() << '\t';
  std::cout << "myWReqsts.capacity() at constructor: " << myWReqsts.capacity() << '\n';
  */
  //assert(myWReqsts.size() > 0);
}

StreamClient::~StreamClient() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing StreamClient class..." << '\n';
#endif
}

/*
ClientTID StreamClient::maximumWriteTID() const {
  //std::cout << "Inside maximumWriteTID()..." << '\n';
  return myWReqsts.front();
}

ClientTID StreamClient::minimumWriteTID() const {
  /*
  std::cout << "Inside minimumWriteTID()..." << '\t' << "Client token: " << myToken << '\n';
  assert(myWReqsts.size() > 0);
  //
  return myWReqsts.back();
}
*/

ClientToken StreamClient::token() const {
  return myToken;
}


ClientTID StreamClient::getNewResponseTID() {
  return ++myLastResp;
}

int StreamClient::addReader(ClientToken ot) {
  myReaders.push_back(ot);
  for (StreamQueue::iterator sv = myQueue.begin(); sv != myQueue.end(); sv++) {
    sv->addReadPermission(ot);
  }
#if defined(DEBUG) && VERBOSENESS > 0
  std::cout << "[DEBUG] Added reader " << ot << " for client " << myToken << "..." << '\n';
#endif
  return 0; // TODO Add error management, returning -1...
}

int StreamClient::removeReader(ClientToken ot) {
  bool found = false;
  for (ReadManager::iterator t = myReaders.begin(); t != myReaders.end(); t++) {
    if (*t == ot) {
      myReaders.erase(t);
      found = true;
    }
  }
  if (!(found)) {
    std::cerr << RED << "[ERROR] Impossible to remove reader (token not found)!" << RESET << '\n';
    return -1;
  }
  for (StreamQueue::iterator sv = myQueue.begin(); sv != myQueue.end(); sv++) {
    sv->removeReadPermission(ot);
  }
#if defined(DEBUG) && VERBOSENESS > 0
  std::cout << "[DEBUG] Removed reader " << ot << " for client " << myToken << "..." << '\n';
#endif
  return 0;
}

/*
AudioVector StreamClient::readVector(ClientToken ot) {
  AudioVector v(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0);
  if (!(myRReqsts.count(ot) > 0)) {
    std::cerr << RED << "[ERROR] Not found read permission for client " << ot << "for the vector of client " << myToken << "!" << RESET << '\n';
    return v;
  }
  ClientTID prtid = myRReqsts[ot];  // Get the last TID stored for read requests from the current peer client...
  ClientVID vid = (prtid + 1) % myBuff.size();
  SharedVector& sv = myBuff[vid];  // Refer to shared vector...
  v = sv.read(ot);
  if (sv.isToClear()) { // Check if vector should be cleared...
    sv.clear();
  }
  updateReadHistory(ot, prtid + 1);
  return v;
}


void StreamClient::writeVector(ClientToken mt, ClientTID wtid, AudioVector& v) {
  assert(mt == myToken);
  assert(wtid != myLastWReq);
  //assert(wtid > static_cast<ClientTID>(maximumWriteTID() - myBuff.size())); // XXX Raise segmentation fault...
  ClientVID vid = wtid % myBuff.size();
#if defined(DEBUG) && VERBOSENESS > 0
  //debugPrintVIDLimits(vid); // OKAY, TESTED!
#endif
  SharedVector& sv = myBuff[vid];
  sv.write(v);
  updateWriteHistory(wtid);
}
*/

void StreamClient::insertVector(ClientToken mt, ClientTID wtid, AudioVector& v) {
  assert(mt == myToken);
  if (myReaders.size() > 0) {
    StreamVector sv;
    sv.setTID(wtid);
    sv.setAudioVector(v);
    for (ReadManager::iterator t = myReaders.begin(); t != myReaders.end(); t++) {
      sv.addReadPermission(*t);
    }
    myQueue.push_back(sv);
    myQueue.sort(comp);
  }
}

AudioVector StreamClient::retrieveVector(ClientToken ot) {
  AudioVector v(AUDIO_VECTOR_SIZE * NUM_CHANNELS, 0.0);
  bool found;
  for (StreamQueue::iterator sv = myQueue.begin(); sv != myQueue.end(); sv++) {
    if (sv->isReadableBy(ot)) {
      found = true;
      v = sv->readVector(ot);
      if (sv->isDeletable()) {
#if defined(DEBUG) && VERBOSENESS > 0
        std::cout << "[DEBUG] Deleting vector from queue..." << '\n';
#endif
        myQueue.erase(sv);
      }
      break;
    }
  }
  if (!(found)) {
    std::cerr << RED << "[ERROR] No readable vectors on queue!" << RESET << '\n';
  }
  return v;
}

/*
void StreamClient::debugPrintReadStatus() {
  std::cout << CYAN << "Client " << myToken << " has the following read histo: " << '\n';
  for (ReadHistory::iterator i = myRReqsts.begin(); i != myRReqsts.end(); i++) {
    std::cout << "- client " << i->first << " has TID " << static_cast<int>(i->second) << '\t';
  }
  std::cout << RESET << '\n';
}


void StreamClient::debugPrintVIDLimits(ClientVID vid) {
  static ClientVID maxVID = 0;  // TODO Remove these static variables for debug purpose only...
  static ClientVID minVID = AUDIO_BUFFER_SIZE / AUDIO_VECTOR_SIZE;
  if (vid > maxVID) {
    maxVID = vid;
    std::cout << "New max. VID: " << maxVID << '\n';
  }
  if (vid < minVID) {
    minVID = vid;
    std::cout << "New min. VID: " << minVID << '\n';
  }
}

void StreamClient::updateReadHistory(ClientToken ot, ClientTID rtid) {
  assert(rtid == myRReqsts[ot] + 1);
  myRReqsts[ot] = rtid;
}

void StreamClient::updateWriteHistory(ClientTID wtid) {
  myWReqsts.push_back(wtid);
  std::sort(myWReqsts.begin(), myWReqsts.end(), std::greater<ClientTID>());
  while (myWReqsts.size() > 1) {
    ClientTID m = myWReqsts[myWReqsts.size() - 2];
    ClientTID n = myWReqsts[myWReqsts.size() - 1];
    if (n + 1 == m) {
      myWReqsts.pop_back();
    } else {
      break;
    }
  }
  assert(myWReqsts.size() > 0);
}
*/
