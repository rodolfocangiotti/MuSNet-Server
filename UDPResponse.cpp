#include <cassert>
#include <iostream>
#include "UDPResponse.h"
#include "commons.h"
#include "prettyprint.h"
#include "utils.h"

UDPResponse::UDPResponse(Manager& m):
  myManager(m) {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Constructing UDPResponse class..." << '\n';
#endif
}

UDPResponse::~UDPResponse() {
#if defined(DEBUG) && VERBOSENESS > 2
  std::cout << getUTCTime() << " [DEBUG] Destructing UDPResponse class..." << '\n';
#endif
}

void UDPResponse::operator()(RequestInfo& r) {
  struct sockaddr_in addrss = r.address();
  socklen_t addrssLen = r.addressLength();
  SocketFD sockFD = r.fileDescriptor();
  Time e = r.receiptTime();
  const UDPDatagram& reqstDatagram = r.referDatagram();

  if (reqstDatagram.header() == AUDIO_STREAM_DATA) {
    ClientToken t = reqstDatagram.token();
    ClientTID reqstTID = reqstDatagram.tid();
    ClientTID respTID = 0;
    AudioVector fromClient = reqstDatagram.streamCopy();
    AudioVector toClient;
    {
      std::lock_guard<std::mutex> l(myManager.referMutex());
      if (myManager.updateClientStream(t, reqstTID, fromClient) < 0) {  // Save stream from client...
        std::cerr << getUTCTime() << RED << " [ERROR] Error updating client stream!" << RESET << '\n';
      }
      toClient = myManager.getOtherClientStreams(t);  // Compute the response stream to client...
      respTID = myManager.getClientResponseTID(t);
    }
    if (respTID > 0) {
      // Use another reference to the same object to change its content...
      UDPDatagram& writableReqstDatagram = r.referWritableDatagram();
      writableReqstDatagram.buildAudioStream(t, respTID, toClient);
      // Use the constant referece to object in order to send the datagram to client...
      int bytes = sendto(sockFD, (const char*) reqstDatagram.rawBuffer(), UDP_BUFFER_SIZE, 0, (const struct sockaddr*) &addrss, addrssLen);
      if (bytes < 0) { // TODO Test it!
        perror("sendto()");
      }
    }

  } else {
    std::cerr << getUTCTime() << RED << " [ERROR] Not consistent header of UDP datagram!" << RESET << '\n';
  }

}
