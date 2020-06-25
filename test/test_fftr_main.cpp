//
// Created by e on 6/25/20.
//

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include <sndfile.hh>

#include "kiss_fft.h"
#include "tools/kiss_fftr.h"

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

vector<float> readFile(const string &path, int &format, int &sr) {
  SndfileHandle file(path);
  int nf = file.frames();
  if (nf < 1) {
    cerr << "no frames in input file; exiting" << endl;
    exit(1);
  }
  vector<float> buf(nf);
  file.readf(buf.data(), nf);
  format = file.format();
  sr = file.samplerate();
  return buf;
}



// write mono soundfile, given float vector
bool writeBuffer(const string &path, vector<float> &buf, int format, int sr) {
  SndfileHandle file(path, SFM_WRITE, format, 1, sr);
  file.command(SFC_SET_CLIPPING, NULL, SF_TRUE);
  const float *data = buf.data();
  const size_t frames = buf.size();
  auto nf = file.writef(data, frames);
  cout << "wrote " << nf << " frames" << endl;
  return true;
}

int main(int argc, char *argv[]) {
  int format;
  int sr;
  std::vector<float> input = readFile(std::string(argv[1]), format, sr);


  int nfft = input.size();

  // forward transform
  kiss_fftr_cfg fftState = kiss_fftr_alloc(nfft, 0, NULL, NULL);
  kiss_fft_cpx fftData[nfft];
  kiss_fftr(fftState, input.data(), fftData);
  kiss_fftr_free(fftState);
  kiss_fft_cleanup();

  std::cout << "max value of input: " << *std::max_element(input.begin(), input.end()) << std::endl;;

  // inverse transform
  std::vector<float> output(input.size());
  fftState = kiss_fftr_alloc(nfft, 1, NULL, NULL);
  kiss_fftri(fftState, fftData, output.data());
  kiss_fftr_free(fftState);
  kiss_fft_cleanup();


  std::cout << "max value of output: " << *std::max_element(output.begin(), output.end()) << std::endl;

  float scale = 1.f / static_cast<float>(nfft);
  for (int i=0; i<nfft; ++i) {
    output[i] *= scale;
  }
  writeBuffer(std::string(argv[2]), output, format, sr);

}

