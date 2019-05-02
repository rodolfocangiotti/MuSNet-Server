#include <cassert>
#include <iostream>
#include "UDPResponse.h"
#include "commons.h"
#include "prettyprint.h"

UDPResponse::UDPResponse(Manager& m):
  myManager(m) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing UDPResponse class..." << std::endl;
#endif
}

UDPResponse::~UDPResponse() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing UDPResponse class..." << std::endl;
#endif
}

void UDPResponse::operator()(RequestInfo& r) {
  struct sockaddr_in addrss = r.address();
  socklen_t addrssLen = r.addressLength();
  SocketFD sockFD = r.fileDescriptor();
  const UDPDatagram& reqstDatagram = r.referDatagram();

  if (reqstDatagram.header() == AUDIO_STREAM_DATA) {
    std::lock_guard<std::mutex> l(myMutex);
    StreamClient::Token t = reqstDatagram.token();
    StreamClient::TID tid = reqstDatagram.tid();
    StreamClient::TID storedTID = myManager.getClientTID(t);
    if (storedTID < 0) {
      std::cerr << RED << "[ERROR] TID not found!" << RESET << std::endl;
      return;
    }
    // XXX When a client sends the disconnection request to TCP node , the UDP node can still have some packets from that client on queue. Assertion, then is not valid: a client can have been just disconnected but the last UDP packet is computed right now!

    if (tid > storedTID) {
      AudioVector toClient(AUDIO_VECTOR_SIZE, 0.0); // XXX Initialization should not be necessary!
      // TODO Check it!
      if (myManager.updateClientTID(t, tid) < 0) {
        std::cerr << RED << "[ERROR] Error updating client TID!" << RESET << std::endl;
        return;
      }
      AudioVector fromClient = reqstDatagram.streamCopy();
      if (myManager.updateClientStream(t, fromClient) < 0) {  // Save stream from client...
        std::cerr << RED << "[ERROR] Error updating client stream!" << RESET << std::endl;
        return;
      }
      toClient = myManager.getOtherClientStreams(t);  // Build a new datagram and send it to client...
      // CHECKED!
      // Use another reference to the same object to change its content...
      UDPDatagram& writableReqstDatagram = r.referWritableDatagram();
      writableReqstDatagram.buildAudioStream(toClient, t, tid);
      // Use the constant referece to object in order to send the datagram to client...
      // CHECKED!
      int bytes = ::sendto(sockFD, (const char*) reqstDatagram.rawBuffer(), UDP_BUFFER_SIZE, 0, (const struct sockaddr*) &addrss, addrssLen);
      if (bytes < 0) {
        perror("send");
      }
    } else {
      std::cerr << RED << "[ERROR] Not consistent TID comparison for client " << t << ": " << tid << "-" << storedTID << " (current-last)!" << RESET << std::endl;
      myManager.clearClientStream(t); // TODO Check it
    }

  } else {
    std::cerr << RED << "[ERROR] Not consistent header of UDP datagram!" << RESET << std::endl;
  }

}
