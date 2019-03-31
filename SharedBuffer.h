#ifndef SHARED_BUFFER_H
#define SHARED_BUFFER_H

#include <mutex>
#include <vector>
#include "types.h"

class SharedBuffer {
public:
  typedef uint64_t Index;
  typedef uint64_t Size;
  // ********************
  SharedBuffer(Size s);
  ~SharedBuffer();
  // ********************
  AudioSample operator[](Index i);
  AudioSample at(Index i);
  AudioVector copy() const;
  Size size() const;
  // ********************
  void update(const AudioVector& o);
  void clear();
private:
  AudioVector v;
  //std::mutex m;
};

#endif
