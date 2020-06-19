#include "args.hpp"
#include "buffer_operations.hpp"
#include "interpolate.hpp"
#include "io.hpp"

using namespace wttool;


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
