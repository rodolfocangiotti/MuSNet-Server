#ifndef COMMONS_H
#define COMMONS_H

#define AUDIO_VECTOR_SIZE 512
#define NUM_CHANNELS 2

#define MAX_TCP_CONNECTIONS 16
#define MAX_UDP_CONNECTIONS 16  // Check if this directive is necessary...
#define TCP_BUFFER_SIZE 3 // That is sizeof (Header) + sizeof (Token)...
#define UDP_BUFFER_SIZE 8201  // That is sizeof (Header) + sizeof (Token) + sizeof (ID) + sizeof (Size) + sizeof (AudioSample) * AUDIO_VECTOR_SIZE * NUM_CHANNELS...

#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

enum headers {
  OKAY,
  ERROR,
  ENTRY_REQUEST,
  EXIT_REQUEST,
  AUDIO_STREAM_DATA
};

#endif
