//
// Created by e on 6/18/20.
//

#ifndef WTTOOL_ARGS_HPP
#define WTTOOL_ARGS_HPP

#include "cxxopts.hpp"
#include "headers.hpp"

namespace wttool {

cxxopts::ParseResult parse(int argc, char *argv[]) {
  using namespace std;
  try {
    cxxopts::Options options(argv[0], " - wavetable file utility");

    options.add_options()("help", "print help")(
        "i, input", "input soundfile",
        cxxopts::value<string>()->default_value("input.wav"))(
        "o, output", "output soundfile",
        cxxopts::value<string>()->default_value("output.wav"))(
        "n, length", "force output length (resampling)",
        cxxopts::value<int>()->default_value("0"))(
        "p, poweroftwo", "make output length a power of two",
        cxxopts::value<bool>()->default_value("false"))(
        "s, supercollider", "use SuperCollider format",
        cxxopts::value<bool>()->default_value("false"))(
        "t, interpolation", "interpolation order (if resampling)",
        cxxopts::value<int>()->default_value("1"))(
        "z, zero", "trim to zero-crossings",
        cxxopts::value<bool>()->default_value("false"))(
        "m, multi_count", "multisample interval count",
        cxxopts::value<int>()->default_value("0"))(
        "v, multi_interval", "multisample intervals per octave",
        cxxopts::value<int>()->default_value("1"));
    return options.parse(argc, argv);
  } catch (const cxxopts::OptionException &e) {
    cout << "error parsing options: " << e.what() << endl;
    exit(1);
  }
}

} // namespace wttool

#endif // WTTOOL_ARGS_HPP
