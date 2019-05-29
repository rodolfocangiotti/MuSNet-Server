#ifndef COMMONS_H
#define COMMONS_H

#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_VECTOR_SIZE 256
#define NUM_CHANNELS 2

#define MAX_TCP_CONNECTIONS 16
#define TCP_BUFFER_SIZE 3 // That is sizeof (Header) + sizeof (Token)...
#define UDP_BUFFER_SIZE 1033  // That is sizeof (Header) + sizeof (Token) + sizeof (TID) + sizeof (Size) + sizeof (AudioSample) * AUDIO_VECTOR_SIZE * NUM_CHANNELS...
// Header, token, TID and size are 9 bytes...

#define MIN_QUEUE_LENGTH 3  // Considering sample rate and vector size, this directive adds ~20 milliseconds delay in client applications...
#define MAX_QUEUE_LENGTH 64

#ifndef NUM_THREADS
#define NUM_THREADS 1
#endif

#if defined(DEBUG) && !(defined(VERBOSENESS))
#define VERBOSENESS 1
#endif

enum headers {
  OKAY_RESPONSE,
  ERROR_RESPONSE,
  ENTRY_REQUEST,
  EXIT_REQUEST,
  AUDIO_STREAM_DATA
};

#endif
