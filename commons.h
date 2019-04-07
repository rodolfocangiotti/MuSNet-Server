#ifndef COMMONS_H
#define COMMONS_H

#define AUDIO_VECTOR_SIZE 512
#define NUM_CHANNELS 2

#define MAX_TCP_CONNECTIONS 16
#define MAX_UDP_CONNECTIONS 16
#define TCP_BUFFER_SIZE 3 // That is sizeof (Header) + sizeof (Token)...
#define UDP_BUFFER_SIZE 8201  // That is sizeof (Header) + sizeof (Token) + sizeof (ID) + sizeof (Size) + sizeof (AudioSample) * AUDIO_VECTOR_SIZE * NUM_CHANNELS...

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
