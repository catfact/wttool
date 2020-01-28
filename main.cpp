#include <functional>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

#include <sndfile.hh>
#include <cmath>
#include "lib/cxxopts.hpp"

using std::vector;
using std::string;
using std::cout;
using std::cerr;
using std::endl;

//------------------------------------------------
//--- argument processing

cxxopts::ParseResult parse(int argc, char *argv[]) {
    try {
        cxxopts::Options options(argv[0], " - wavetable file utility");

        options.add_options()
                ("help", "print help")
                ("i, input", "input soundfile", cxxopts::value<string>()->default_value("input.wav"))
                ("o, output", "output soundfile", cxxopts::value<string>()->default_value("output.wav"))
                ("n, length", "force output length (resampling)", cxxopts::value<int>()->default_value("0"))
                ("p, poweroftwo", "make output length a power of two", cxxopts::value<bool>()->default_value("false"))
                ("s, supercollider", "use SuperCollider format", cxxopts::value<bool>()->default_value("false"))
                ("t, interpolation", "interpolation order (if resampling)", cxxopts::value<int>()->default_value("3"))
                ("z, zero", "trim to zero-crossings", cxxopts::value<bool>()->default_value("false"));
        return options.parse(argc, argv);
    } catch (const cxxopts::OptionException &e) {
        cout << "error parsing options: " << e.what() << endl;
        exit(1);
    }
}

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
    SndfileHandle file(path, SFM_WRITE, format, 1, sr);
    file.command(SFC_SET_CLIPPING, NULL, SF_TRUE);
    const float* data = buf.data();
    const size_t frames = buf.size();
    auto nf = file.writef(data, frames);
    cout << "wrote " << nf << " frames" << endl;
}

//------------------------------------------------
//-- interpolation

typedef std::function<float(const vector<float>&, int, double)> InterpFunc;

// interpolate, zero-order
float interpHold(const vector<float>& inBuf, size_t outputFrame, double ratio) {
    auto inFrame = static_cast<size_t>(static_cast<double>(outputFrame) * ratio);
    return inBuf[inFrame];
};

// interpolate, linear
float interpLinear(const vector<float>& inBuf, size_t outputFrame, double ratio) {
    double inPos = static_cast<double>(outputFrame) * ratio;
    auto inFrame0 = static_cast<size_t>(inPos);
    size_t inFrame1 = inFrame0 >= (inBuf.size()-1) ? inFrame0 : inFrame0 + 1;
    auto x = static_cast<float>(inPos - static_cast<double>(inFrame0));
    return inBuf[inFrame0] + x * (inBuf[inFrame1] - inBuf[inFrame0]);
};

// interpolate, cubic
float interpCubic(const vector<float>& inBuf, size_t outputFrame, double ratio) {
    auto inFrame = static_cast<size_t>(static_cast<double>(outputFrame) * ratio);
    double inPos = static_cast<double>(outputFrame) * ratio;
    auto inFrame1 = static_cast<size_t>(inPos);
    size_t inFrame0 = inFrame1 > 0 ? inFrame1 - 1 : 0;
    size_t inFrame2 = inFrame1 >= (inBuf.size()-1) ? inFrame1 : inFrame1 + 1;
    size_t inFrame3 = inFrame2 >= (inBuf.size()-1) ? inFrame2 : inFrame2 + 1;
    float y0 = inBuf[inFrame0];
    float y1 = inBuf[inFrame1];
    float y2 = inBuf[inFrame2];
    float y3 = inBuf[inFrame3];
    double x = inPos - static_cast<double>(inFrame1);
    double y = (((0.5 * (y3 - y0) + 1.5 * (y1 - y2)) * x + (y0 - 2.5 * y1 + 2. * y2 - 0.5 * y3)) * x + 0.5 * (y2 - y0)) * x + y1;
    return static_cast<float>(y);
};


//------------------------------------------------
//-- buffer processing

vector<float> resample(const vector<float> &inBuf, int outputFrames, const InterpFunc& interpFunc) {
    vector<float> outBuf(outputFrames);
    double ratio = static_cast<double>(inBuf.size()) / static_cast<double>(outputFrames);
    for (size_t outFrame=0; outFrame<outputFrames; ++outFrame) {
        outBuf[outFrame] = interpFunc(inBuf, outFrame, ratio);
    }
    return outBuf;
}

// convert float vector to position-delta format, returning new vector of 2x size
vector<float> convertToScWavetable(vector<float> in) {
    size_t framesIn = in.size();
    size_t framesOut = framesIn * 2;
    vector<float> out(framesOut);
    for (size_t i=0; i<framesIn; ++i) {
        out[i*2] = in[i];
        out[i*2 + 1] = in[(i+1)%framesIn] - in[i];
    }
    return out;
}

size_t nextPowerOfTwo(size_t x) {
    size_t y = x;
    while ((y & (y-1)) != 0) {
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
    size_t end = inBuf.size()-1;
    while (!isNearZero(inBuf[start])) {
        ++start;
    }
    while (!isNearZero(inBuf[end])) {
        --end;
    }
    size_t len = end-start;
    if (len < 1) {
        return inBuf;
    }
    auto outBuf = vector<float>(len);
    int i=0;
    for (int frame=start; frame<end; ++frame) {
        outBuf[i] = inBuf[frame];
    }
    return outBuf;
}


//------------------------------------------------
//--- YE MAIN

int main(int argc, char *argv[]) {
    auto args = parse(argc, argv);

    // check arguments
    cout << "input: " << args["input"].as<string>() << endl;
    cout << "output: " << args["output"].as<string>() << endl;
    cout << "length: " << args["length"].as<int>() << endl;
    cout << "poweroftwo: " << args["poweroftwo"].as<bool>() << endl;
    cout << "interpolation: " << args["interpolation"].as<int>() << endl;
    cout << "zero: " << args["zero"].as<bool>() << endl;
    cout << "supercollider: " << args["supercollider"].as<bool>() << endl;

    int format;
    int sr;
    vector<float> inBuf = readFile(args["input"].as<string>(), format, sr);

    size_t inFrames = inBuf.size();
    size_t outFrames = inFrames;

    int arg_len = args["length"].as<int>();
    if (arg_len != 0) {
        outFrames = arg_len;
    }

    if (args["poweroftwo"].as<bool>()) {
        outFrames = nextPowerOfTwo(outFrames);
    }

    std::vector<float> outBuf;

    if (args["zero"].as<bool>()) {
        inBuf = trimToZeros(inBuf);
    }

    if (outFrames == inFrames) {
        outBuf = inBuf;
    } else {
        InterpFunc interpFunc;
        switch (args["interpolation"].as<int>()) {
            case 0:
                interpFunc = interpHold;
                break;
            case 1:
                interpFunc = interpLinear;
                break;
            case 2:
            default:
                interpFunc = interpCubic;
                break;
        }
        outBuf = resample(inBuf, outFrames, interpFunc);
    }


    if (args["supercollider"].as<bool>()) {
        outBuf = convertToScWavetable(outBuf);
    }

    writeBuffer(args["output"].as<string>(), outBuf, format, sr);
    return 0;
}
