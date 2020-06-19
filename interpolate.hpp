//
// Created by e on 6/18/20.
//

#ifndef WTTOOL_INTERPOLATE_HPP
#define WTTOOL_INTERPOLATE_HPP

#include "headers.hpp"

namespace wttool {
//------------------------------------------------
//-- interpolation

typedef std::function<float(const vector<float> &, int, double)> InterpFunc;

// interpolate, zero-order
float interpHold(const vector<float> &inBuf, size_t outputFrame, double ratio) {
  auto inFrame = static_cast<size_t>(static_cast<double>(outputFrame) * ratio);
  return inBuf[inFrame];
};

// interpolate, linear
float interpLinear(const vector<float> &inBuf, size_t outputFrame,
                   double ratio) {
  double inPos = static_cast<double>(outputFrame) * ratio;
  auto inFrame0 = static_cast<size_t>(inPos);
  size_t inFrame1 = inFrame0 >= (inBuf.size() - 1) ? inFrame0 : inFrame0 + 1;
  auto x = static_cast<float>(inPos - static_cast<double>(inFrame0));
  return inBuf[inFrame0] + x * (inBuf[inFrame1] - inBuf[inFrame0]);
};

// interpolate, cubic
float interpCubic(const vector<float> &inBuf, size_t outputFrame,
                  double ratio) {
  auto inFrame = static_cast<size_t>(static_cast<double>(outputFrame) * ratio);
  double inPos = static_cast<double>(outputFrame) * ratio;
  auto inFrame1 = static_cast<size_t>(inPos);
  size_t inFrame0 = inFrame1 > 0 ? inFrame1 - 1 : 0;
  size_t inFrame2 = inFrame1 >= (inBuf.size() - 1) ? inFrame1 : inFrame1 + 1;
  size_t inFrame3 = inFrame2 >= (inBuf.size() - 1) ? inFrame2 : inFrame2 + 1;
  float y0 = inBuf[inFrame0];
  float y1 = inBuf[inFrame1];
  float y2 = inBuf[inFrame2];
  float y3 = inBuf[inFrame3];
  double x = inPos - static_cast<double>(inFrame1);
  double y = (((0.5 * (y3 - y0) + 1.5 * (y1 - y2)) * x +
               (y0 - 2.5 * y1 + 2. * y2 - 0.5 * y3)) *
                  x +
              0.5 * (y2 - y0)) *
                 x +
             y1;
  return static_cast<float>(y);
};
}
#endif // WTTOOL_INTERPOLATE_HPP
