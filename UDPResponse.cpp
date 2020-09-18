#include <cassert>
#include <iostream>
#include "Chrono.h"
#include "Console.h"
#include "UDPResponse.h"
#include "commons.h"
#include "prettyprint.h"
#include "utils.h"

double average(double x) {
  /*
  static double sum = 0.0;
  static double cntr = 0.0;
  sum += x;
  cntr += 1.0;
  return sum / cntr;
  */
  static std::vector<double> mem(16384, 0.0);
  static int i = 0;
  i = i < mem.size() ? i : 0;
  mem[i] = x;
  i++;
  double sum = 0.0;
  for (int j = 0; j < mem.size(); j++) {
    sum += mem[j];
  }
  return sum / static_cast<double>(mem.size());
}

UDPResponse::UDPResponse(Manager& m):
  myManager(m) {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Constructing UDPResponse class...");
#endif
}

UDPResponse::~UDPResponse() {
#if defined(DEBUG) && VERBOSENESS > 2
  Console::log(getUTCTime() + " [DEBUG] Destructing UDPResponse class...");
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
        std::cerr << getUTCTime() + RED << " [ERROR] Error updating client stream!" << RESET << '\n';
      }
      toClient = myManager.getOtherClientStreams(t);  // Compute the response stream to client...
      respTID = myManager.getClientResponseTID(t);
    }
    if (respTID > 0) {
      // Use another reference to the same object to change its content...
      UDPDatagram& writableReqstDatagram = r.referWritableDatagram();
      writableReqstDatagram.buildAudioStream(t, respTID, toClient);
      double d = Chrono::timeDelta();
      Console::log("Computation time: " + str(d * 1e-6) + " msecs. (average: " + str(average(d) * 1e-6) + ")");
      // Use the constant referece to object in order to send the datagram to client...
      int bytes = sendto(sockFD, (const char*) reqstDatagram.rawBuffer(), UDP_BUFFER_SIZE, 0, (const struct sockaddr*) &addrss, addrssLen);
      if (bytes < 0) { // TODO Test it!
        perror("sendto()");
      }
    }

  } else {
    std::cerr << getUTCTime() + RED << " [ERROR] Not consistent header of UDP datagram!" << RESET << '\n';
  }

}
