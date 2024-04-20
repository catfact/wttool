//
// Created by e on 6/18/20.
//

#ifndef WTTOOL_BUFFER_OPERATIONS_HPP
#define WTTOOL_BUFFER_OPERATIONS_HPP

#include "headers.hpp"
#include "interpolate.hpp"

namespace wttool {

vector<float> resample(const vector<float> &inBuf, int outputFrames,
                       const InterpFunc &interpFunc) {
  vector<float> outBuf(outputFrames);
  double ratio =
      static_cast<double>(inBuf.size()-1) / static_cast<double>(outputFrames-1);
  for (size_t outFrame = 0; outFrame < outputFrames; ++outFrame) {
    outBuf[outFrame] = interpFunc(inBuf, outFrame, ratio);
  }
  return outBuf;
}

// convert float vector to position-delta format, returning new vector of 2x size
vector<float> convertToScWavetable(vector<float> in) {
  size_t framesIn = in.size();
  size_t framesOut = framesIn * 2;
  vector<float> out(framesOut);
  const float *src = in.data();
  float *dst = out.data();
    for (size_t i = 0; i < framesIn; ++i) {
        float a = src[i];
        float b = src[(i + 1) % framesIn];
        *dst++ = 2.f * a - b;
        *dst++ = b - a;
    }
  return out;
}

size_t nextPowerOfTwo(size_t x) {
  size_t y = x;
  while ((y & (y - 1)) != 0) {
    ++y;
  }
  return y;
}

// helper
bool isNearZero(float x) {
  return std::fabs(x) <= (std::numeric_limits<float>::epsilon() * 2);
}

// trim beginning and end of float vector to first occurrence of "near zero"
// returns a new vector
vector<float> trimToZeros(const vector<float> &inBuf) {
  size_t start = 0;
  size_t end = inBuf.size() - 1;
  while (!isNearZero(inBuf[start])) {
    ++start;
  }
  while (!isNearZero(inBuf[end])) {
    --end;
  }
  size_t len = end - start;
  if (len < 1) {
    return inBuf;
  }
  auto outBuf = vector<float>(len);
  int i = 0;
  for (int frame = start; frame < end; ++frame) {
    outBuf[i] = inBuf[frame];
  }
  return outBuf;
}

}


#endif // WTTOOL_BUFFER_OPERATIONS_HPP
