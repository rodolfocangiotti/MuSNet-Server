#ifndef COMMONS_H
#define COMMONS_H

#define AUDIO_BUFFER_SIZE 1024  // That is vector size multiplied by 4...
#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_VECTOR_SIZE 256
#define NUM_CHANNELS 2

//#define MAX_TID_OFFSET 1  // TODO Check it!

#define MAX_TCP_CONNECTIONS 16
#define MAX_UDP_CONNECTIONS 16  // Check if this directive is necessary...
#define TCP_BUFFER_SIZE 3 // That is sizeof (Header) + sizeof (Token)...
#define UDP_BUFFER_SIZE 4105  // That is sizeof (Header) + sizeof (Token) + sizeof (TID) + sizeof (Size) + sizeof (AudioSample) * AUDIO_VECTOR_SIZE * NUM_CHANNELS...
// Header, token, TID and size are 9 bytes...

#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

#if defined(DEBUG) && !(defined(VERBOSENESS))
#define VERBOSENESS 1
#endif

enum headers {
  OKAY,
  ERROR_RESPONSE,
  ENTRY_REQUEST,
  EXIT_REQUEST,
  AUDIO_STREAM_DATA
};

#endif
