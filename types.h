#ifndef TYPES_H
#define TYPES_H

#include <vector>

typedef double AudioSample;
typedef std::vector<AudioSample> AudioVector;
typedef uint8_t Byte;
typedef std::vector<Byte> Buffer;

typedef uint16_t PortNum;
typedef int SocketFD;

#endif
