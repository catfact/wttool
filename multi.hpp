//
// Created by e on 6/18/20.
//
// multisampling functions

#ifndef WTTOOL_MULTI_HPP
#define WTTOOL_MULTI_HPP

#include <memory>
#include "kiss_fftr.h"

class DftData {
  int nbins; // number of real bins
  std::unique_ptr<kiss_fft_cpx[]> data;

private:
  void init(int nb) {
    nbins = nb;
    data = std::make_unique<kiss_fft_cpx[]>(nbins);
  }

public:
  // construct an empty DftData given number of bins
  DftData(int n) { init(n); }


  // construct a DftData with analysis of an audio buffer
  // frame count must be even!
  DftData(const float* buf, int nframes) {
    assert(nframes % 2 == 0);
    int nfft = nframes;
    int nb = nfft / 2 + 1;
    init(nb);
    // perform the transform
    kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 0, 0, 0);
    kiss_fftr(cfg, buf, data.get());
    free(cfg);
  }


  // brickwall copy constructor
  // accepts original dft data structure and highest bin number
  DftData (DftData &original, int highestBin) {
    init(original.nbins);
    kiss_fft_cpx *src = original.data.get();
    kiss_fft_cpx *dst = data.get();
    for (int i = 0; i < nbins; ++i) {
      if (i <= highestBin) {
        dst[i] = src[i];
      } else {
        dst[i] = {0, 0};
      }
    }
  }

  void output(float* buf, int nframes) {
    int nfft = (nbins-1) * 2;
    assert (nframes == nfft);
    kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 1, 0, 0);
    kiss_fftri(cfg, data.get(), buf);
    free(cfg);
    kiss_fft_cleanup();
  }

};




#endif // WTTOOL_MULTI_HPP
