#include <cassert>
#include <iostream>
#include "UDPResponse.h"
#include "commons.h"
#include "prettyprint.h"

UDPResponse::UDPResponse(Manager& m):
  myManager(m) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Constructing UDPResponse class..." << '\n';
#endif
}

UDPResponse::~UDPResponse() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << "[DEBUG] Destructing UDPResponse class..." << '\n';
#endif
}

void UDPResponse::operator()(RequestInfo& r) {
  struct sockaddr_in addrss = r.address();
  socklen_t addrssLen = r.addressLength();
  SocketFD sockFD = r.fileDescriptor();
  const UDPDatagram& reqstDatagram = r.referDatagram();

  if (reqstDatagram.header() == AUDIO_STREAM_DATA) {
    std::lock_guard<std::mutex> l(myMutex);
    ClientToken t = reqstDatagram.token();
    ClientTID reqstTID = reqstDatagram.tid();
    /*
    ClientTID storedTID = myManager.getClientTID(t);
    if (storedTID < 0) {
      std::cerr << RED << "[ERROR] TID not found!" << RESET << '\n';
      return;
    }
    */
    // XXX When a client sends the disconnection request to TCP node , the UDP node can still have some packets from that client on queue. Assertion, then is not valid: a client can have been just disconnected but the last UDP packet is computed right now!

    //if (tid > storedTID) { // TODO TODO TODO Provisory comment...
      //AudioVector toClient(AUDIO_VECTOR_SIZE, 0.0); // XXX Initialization should not be necessary!
      // TODO Check it!

      AudioVector fromClient = reqstDatagram.streamCopy();
      if (myManager.updateClientStream(t, reqstTID, fromClient) < 0) {  // Save stream from client...
        std::cerr << RED << "[ERROR] Error updating client stream!" << RESET << '\n';
        return;
      }
      AudioVector toClient = myManager.getOtherClientStreams(t);  // Compute the response stream to client...
      ClientTID respTID = myManager.getClientResponseTID(t);
      // Use another reference to the same object to change its content...
      UDPDatagram& writableReqstDatagram = r.referWritableDatagram();
      writableReqstDatagram.buildAudioStream(toClient, t, respTID);
      // Use the constant referece to object in order to send the datagram to client...
      int bytes = ::sendto(sockFD, (const char*) reqstDatagram.rawBuffer(), UDP_BUFFER_SIZE, 0, (const struct sockaddr*) &addrss, addrssLen);
      if (bytes < 0) {
        perror("send");
      }
    /*
    } else {
      std::cerr << RED << "[ERROR] Not consistent TID comparison for client " << t << ": " << tid << "-" << storedTID << " (current-last)!" << RESET << '\n';
      myManager.clearClientStream(t); // TODO Check it
    }
    */

  } else {
    std::cerr << RED << "[ERROR] Not consistent header of UDP datagram!" << RESET << '\n';
  }

}
