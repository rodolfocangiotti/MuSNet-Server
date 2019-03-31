#ifndef COMMONS_H
#define COMMONS_H

#define MAX_TCP_CONNECTIONS 16
#define MAX_UDP_CONNECTIONS 16
#define TCP_BUFFER_SIZE 8
#define UDP_BUFFER_SIZE 8200  // Size of double precision 512 samples stereo audio frame...

enum headers {
  OKAY,
  ERROR,
  ENTRY_REQUEST,
  EXIT_REQUEST,
  CONNECTION_DATA,
  CONNECTION_CHECK,
  AUDIO_STREAM_REQUEST,
  AUDIO_STREAM_DATA
};

enum modes {
  CLIENT,
  SERVER
};

#endif
