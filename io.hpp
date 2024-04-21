//
// Created by e on 6/18/20.
//

#ifndef WTTOOL_IO_HPP
#define WTTOOL_IO_HPP

#include <sndfile.hh>
#include "headers.hpp"

namespace wttool {
//------------------------------------------------
//--- soundfile I/O

// load mono soundfile, returning float vector
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
#if 1
    // at least using SC format, we can expect values outside unity bounds
    // so it's quite important not to clip or wrap
  format = SF_FORMAT_WAV | SF_FORMAT_FLOAT;
#endif
  SndfileHandle file(path, SFM_WRITE, format, 1, sr);
#if 1
    file.command(SFC_SET_CLIPPING, NULL, SF_FALSE);
#else
        file.command(SFC_SET_CLIPPING, NULL, SF_TRUE);
#endif
  const float *data = buf.data();
  const size_t frames = buf.size();
  auto nf = file.writef(data, frames);
  cout << "wrote " << nf << " frames" << endl;
  return true;
}

}
#endif // WTTOOL_IO_HPP
