#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

typedef double AudioSample;
typedef std::vector<AudioSample> AudioVector;
typedef uint8_t Byte;
typedef std::vector<Byte> Buffer;

typedef uint16_t PortNum;
typedef int SocketFD;
typedef std::string Address;

#endif
