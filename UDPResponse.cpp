#include <iostream>
#include "UDPResponse.h"

UDPResponse::UDPResponse(Manager& m):
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
  addrss = r.address();
  addrssLen = r.addressLength();
  std::cout << "Client address: " << inet_ntoa(addrss.sin_addr) << ":" << ntohs(addrss.sin_port) << std::endl;
  std::cout << "Address length: " << addrssLen << std::endl;
}
