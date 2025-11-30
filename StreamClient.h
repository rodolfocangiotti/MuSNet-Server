#ifndef STREAM_CLIENT_H
#define STREAM_CLIENT_H

#include <arpa/inet.h>
#include <cstdint>
#include <list>
#include "StreamVector.h"
#include "types.h"

bool comp(const StreamVector& a, const StreamVector& b);

class StreamClient {
public:
  /*
  typedef int16_t Token;  // XXX These data types have been moved to types.h...
  typedef int16_t TID;
  // ********************
  */
  StreamClient(const ClientToken t);
  ~StreamClient();
  // ********************
  ClientToken token() const;
  bool isWaiting() const;
  const std::list<ClientTID>& tidHistory() const;
  // ********************
  ClientTID getNewResponseTID();
  // ********************
  int addReader(const ClientToken ot);
  int removeReader(const ClientToken ot);
  // ********************
  AudioVector retrieveVector(const ClientToken ot);
  void insertVector(const ClientToken mt, const ClientTID wtid, const AudioVector& v);
  void set_connection_info(const SocketFD fd, const struct sockaddr_in* addr, const socklen_t addr_len);
  void get_connection_info(SocketFD* fd, struct sockaddr_in* addr, socklen_t* addr_len) const;
  bool has_vector_for(const ClientToken ot);
  void set_flag(Flag f);
  Flag get_flag() const;
private:
  typedef std::list<StreamVector> StreamQueue;
  typedef std::list<ClientToken> ReadManager;
  // ********************
  StreamQueue myQueue;
  ReadManager myReaders;
  ClientTID myLastResp;  // Last TID sent to client for fetch/download requests...
  ClientToken myToken;  // Client code...
  // ********************
  int myQueueMaxSize;
  bool waiting;
  std::list<ClientTID> tidHisto;

  SocketFD _socket;
  struct sockaddr_in _address;
  socklen_t _address_length;
  Flag _flag;
};

#endif
