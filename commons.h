#ifndef COMMONS_H
#define COMMONS_H

#include <string>

#define AUDIO_SAMPLE_RATE 44100
#define AUDIO_VECTOR_SIZE 512
#define NUM_CHANNELS 16

#define MAX_TCP_CONNECTIONS 16
#define TCP_BUFFER_SIZE 3 // That is sizeof (Header) + sizeof (Token)...
// TODO TODO TODO
// FOLLOWING VALUE IS TEMPORARY, ONLY FOR TESTING AMBISONIC SIGNALS VIA TCP!
#define UDP_BUFFER_SIZE 16394  // That is sizeof (Header) + sizeof (Token) + sizeof (TID) + sizeof (Size) + sizeof (AudioSample) * AUDIO_VECTOR_SIZE * NUM_CHANNELS + sizeof (Flag)...
// Header, token, TID, size and flag are 10 bytes...
// ****************
// IMPORTANT: UDP packets shouldn't be larger than 1492 bytes, which is the Ethernet MTU (1500) at physical layer minus the overhead (8).
// If that threshold value is exceeded, the UDP packet will be fragmented, letting the data exchange be even less reliable (more subject to wrong order or data loss).
// ****************

#define MIN_QUEUE_LENGTH 2  // Considering sample rate and vector size, this directive adds ~20 milliseconds delay in client applications...
#ifndef MAX_QUEUE_LENGTH
#define MAX_QUEUE_LENGTH 16
#endif

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

enum flags {
  STANDARD_STREAM,
  SEND_ONLY_STREAM,
  RECEIVE_ONLY_STREAM
};

#define str(n) std::to_string(n)

#ifndef REDUDANCY_FACTOR
#define REDUDANCY_FACTOR 1 // 2
#endif

#define UDP_SEND_WAIT 180   // Microseconds...
                            // 110 is a reference value to use 20Mbit/sec bandwidth with 64 sample DSP block...

#ifndef EXPORT_RECORDS_FOLDER
#define EXPORT_RECORDS_FOLDER "/home/ubuntu/muslogs/"
#endif

#endif
