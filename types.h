#ifndef TYPES_H
#define TYPES_H

#include <chrono>
#include <string>
#include <vector>

//typedef double AudioSample;
//typedef float AudioSample;
typedef int16_t AudioSample;

typedef std::vector<AudioSample> AudioVector;
typedef uint8_t Byte;
typedef std::vector<Byte> Buffer;

typedef int16_t ClientToken;
typedef int32_t ClientTID;

typedef uint16_t PortNum;
typedef int SocketFD;
typedef std::string Address;

typedef std::chrono::time_point<std::chrono::system_clock> SystemTime;
typedef std::chrono::time_point<std::chrono::high_resolution_clock> Time;

#endif
