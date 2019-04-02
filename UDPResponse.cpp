#include <iostream>
#include "UDPResponse.h"
#include "commons.h"

UDPResponse::UDPResponse(Manager& m):
  myDatagram(UDP_BUFFER_SIZE),
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
  struct sockaddr_in addrss;
  socklen_t addrssLen;
  SocketFD sockFD;
  addrss = r.address();
  addrssLen = r.addressLength();
  sockFD = r.fileDescriptor();
#ifdef DEBUG
  std::cout << "Client address: " << inet_ntoa(addrss.sin_addr) << ":" << ntohs(addrss.sin_port) << std::endl;
  std::cout << "Address length: " << addrssLen << std::endl;
#endif
  const Buffer b = r.buffer();
  Byte* bp = static_cast<Byte*>(myDatagram.pointWritableBuffer());
  for (int i = 0; i < b.size(); i++) {  // Copy the RequestInfo buffer to UDPDatagram...
    bp[i] = b[i];
  }

  if (myDatagram.header() == AUDIO_STREAM_DATA) {
    UDPDatagram::Token t = myDatagram.token();
    UDPDatagram::ID id = myDatagram.id();
    StreamClient::ID lastID = myManager.getClientID(t);

    if (id > lastID) {
      // Save stream from client...
      myManager.updateClientStream(t, myDatagram.streamCopy());
      // Build a new datagram and send it to client...
      AudioVector toClient = myManager.getOtherClientStreams(t);
      myDatagram.buildAudioStreamResponse(toClient, t, id);
      int bytes = ::sendto(sockFD, myDatagram.rawBuffer(), UDP_BUFFER_SIZE, 0, (const struct sockaddr*) &addrss, addrssLen);
      if (bytes < 0) {
        perror("send");
      }
      myManager.updateClientID(t, id);
    } else {
#ifdef DEBUG
      std::cout << "[DEBUG] NOT CONSISTENT ID COMPARISON!" << std::endl;
#endif
      myManager.clearClientStream(t);
    }

  } else {
    std::cerr << "[ERROR]" << std::endl;
  }
}
