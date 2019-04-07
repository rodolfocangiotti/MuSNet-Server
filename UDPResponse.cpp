#include <iostream>
#include "UDPResponse.h"
#include "commons.h"

UDPResponse::UDPResponse(Manager& m):
  myPayload(UDP_BUFFER_SIZE),
  myManager(m) {
#ifdef DEBUG
  std::cout << "[DEBUG] Constructing UDPResponse class..." << std::endl;
#endif
}

UDPResponse::~UDPResponse() {
#ifdef DEBUG
  std::cout << "[DEBUG] Destructing UDPResponse class..." << std::endl;
#endif
}

void UDPResponse::operator()(const RequestInfo& r) {
  struct sockaddr_in addrss = r.address();
  socklen_t addrssLen = r.addressLength();
  SocketFD sockFD = r.fileDescriptor();
#ifdef DEBUG  // TODO
  /*
  std::cout << "Client socket: " << sockFD << std::endl;
  std::cout << "Client address: " << inet_ntoa(addrss.sin_addr) << ":" << ntohs(addrss.sin_port) << std::endl;
  std::cout << "Address length: " << addrssLen << std::endl;
  */
#endif
  const Buffer b = r.buffer();
  Byte* bp = static_cast<Byte*>(myPayload.pointWritableBuffer());
  for (int i = 0; i < b.size(); i++) {  // Copy the RequestInfo buffer to UDPDatagram...
    bp[i] = b[i];
  }

  if (myPayload.header() == AUDIO_STREAM_DATA) {
    StreamClient::Token t = myPayload.token();
    StreamClient::TID tid = myPayload.tid();
    StreamClient::TID lastTID = myManager.getClientTID(t);

    if (tid > lastTID) {
      AudioVector toClient;
      {
        std::lock_guard<std::mutex> l(myMutex);
        myManager.updateClientID(t, tid);  // XXX MULTITHREADING CAN CREATE ASYNCRONIZATION BETWEEN WHEN THE ID IS READ AND WHEN IT IS UPDATED!
        // MUTEX ADDED!
        // Save stream from client...
        myManager.updateClientStream(t, myPayload.streamCopy());
        // Build a new datagram and send it to client...
        toClient = myManager.getOtherClientStreams(t);
      }
      myPayload.buildAudioStreamResponse(toClient, t, tid);
      int bytes = ::sendto(sockFD, myPayload.rawBuffer(), UDP_BUFFER_SIZE, 0, (const struct sockaddr*) &addrss, addrssLen);
      if (bytes < 0) {
        perror("send");
      }
    } else {
#ifdef DEBUG
      std::cout << "[DEBUG] Not consistent ID comparison!" << std::endl;
#endif
      {
        std::lock_guard<std::mutex> l(myMutex);
        myManager.clearClientStream(t);
      }
    }

  } else {
    std::cerr << "[ERROR] Not consistent header of UDP datagram!" << std::endl;
  }
}
