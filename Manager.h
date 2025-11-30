#ifndef STREAM_MANAGER_H
#define STREAM_MANAGER_H

#include <arpa/inet.h>
#include <cstdint>
#include <list>
#include <mutex>
#include "StreamClient.h"
#include "types.h"

class Manager {
public:
  Manager();
  ~Manager();
  // ********************
  std::mutex& referMutex();
  // ********************
  ClientToken addClient();
  int removeClient(const ClientToken t);
  // ********************
  AudioVector getOtherClientStreams(const ClientToken t);
  ClientTID getClientResponseTID(const ClientToken t);
  int updateClientStream(const ClientToken t, const ClientTID tid, const AudioVector& v);
  const std::list<ClientTID> getTIDHistory(const ClientToken tk);
  bool is_valid_token(const ClientToken token);
  int update_client_connection_info(const ClientToken t, const SocketFD file_descr, const struct sockaddr_in* address, const socklen_t address_len);
  int get_client_connection_info(const ClientToken t, SocketFD* file_descr, struct sockaddr_in* address, socklen_t* address_len);
  int find_response_candidate(ClientToken* result);
  int update_client_flag(const ClientToken t, const Flag f);
  int get_client_flag(const ClientToken t, Flag* f);
private:
  typedef std::list<StreamClient> ClientList;
  // ********************
  void debugPrint();
  // ********************
  ClientList myClients;
  std::mutex myMutex;
  ClientToken myLastToken;
};

#endif
