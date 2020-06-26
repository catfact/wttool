//
// Created by e on 6/18/20.
//
// multisampling functions

#ifndef WTTOOL_MULTI_HPP
#define WTTOOL_MULTI_HPP


// standard
#include <filesystem>
#include <memory>

// 3rd-party
#include "kiss_fftr.h"

// local
#include "io.hpp"

namespace wttool {
class DftData {
  // number of real bins
  int nbins;
  std::unique_ptr<kiss_fft_cpx[]> data;
private:
  void init(int nb) {
    nbins = nb;
    data = std::make_unique<kiss_fft_cpx[]>(nbins);
  }

public:
  // construct an empty DftData given number of frames
  DftData(int n) { init(n); }

  // construct a DftData with analysis of an audio buffer
  // frame count must be even!
  DftData(const float *buf, int nframes) {
    assert(nframes % 2 == 0);
    int nfft = nframes;
    int nb = nfft / 2 + 1;
    init(nb);
    // perform the transform
    kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 0, 0, 0);
    kiss_fftr(cfg, buf, data.get());
    kiss_fftr_free(cfg);
  }

  // brickwall copy constructor
  // accepts original dft data structure and highest bin number
  DftData(DftData &original, int highestBin) {
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


  // brickwall copy constructor
  // accepts original dft data structure and highest bin number
  DftData(DftData &original) {
    init(original.nbins);
    memcpy(data.get(), original.data.get(), sizeof(kiss_fft_cpx) * nbins);
  }

  void output(float *buf, int nframes) {
    int nfft = (nbins - 1) * 2;
    assert(nframes == nfft);
    kiss_fftr_cfg cfg = kiss_fftr_alloc(nfft, 1, 0, 0);
    kiss_fftri(cfg, data.get(), buf);
    kiss_fftr_free(cfg);

    float scale = 1.f / static_cast<float>(nfft);
    for (int i=0; i<nfft; ++i) {
      buf[i] *= scale;
    }
  }

  void write(const std::string &path, int format) {
    int nf = (nbins - 1) * 2;
    std::vector<float> buf(nf);
    output(buf.data(), nf);
    writeBuffer(path, buf, format, nf);
  }

  int numBins() { return nbins; }
};

// export a series of bandlimited waveforms
static void multi_perform(float *input, int nframes,
                          int count, float interval,
                    const std::string &outputFile, int format) {

  auto outputFilePath = std::filesystem::path(outputFile);
  auto ext = outputFilePath.extension();
  auto stem = outputFilePath.stem();
  auto location = outputFilePath.parent_path();
  std::string basePath(location);
  basePath.append(stem);

  DftData baseData(input, nframes);
  int nbins = baseData.numBins();
  float topBin = static_cast<float>(nbins);

  for (int i = 0; i < count; ++i) {
    topBin /= interval;
    int topBinIdx = std::max(0, static_cast<int>(topBin)-1);
    std::cout << "top bin index: " << topBinIdx << std::endl;
    DftData filteredData(baseData, topBinIdx);
    std::string fileName(basePath);
    fileName.append("_");
    fileName.append(std::to_string(i+1));
    fileName.append(ext);
    filteredData.write(fileName, format);
  }
}

} // namespace wttool

#endif // WTTOOL_MULTI_HPP
