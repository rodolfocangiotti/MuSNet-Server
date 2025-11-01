#include <cassert>
#include <iostream>
#include "Console.h"
#include "UDPDatagram.h"
#include "commons.h"
#include "utils.h"

UDPDatagram::UDPDatagram(const MaxSize ms):
  StreamPayload(ms) {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing UDPDatagram class...");
#endif
}

UDPDatagram::~UDPDatagram() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Destructing UDPDatagram class...");
#endif
}

void UDPDatagram::setParentTID(ClientTID tid) {
  myParentTID = tid;
}

ClientTID UDPDatagram::parentTID() const {
  return myParentTID;
}
