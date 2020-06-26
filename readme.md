# wttool

this is a simple command-line utility to process single-cycle waveforms.

## motivation

the main motivator for this tool is a need to convert arbitrary sound files to the format expected by SuperCollider's `Osc` family of wavetable-oscillator UGens.

the first limitation of `Osc` et al, is that the size of their buffers must be a power of two.

if that can be provided, it is still inconvenient to convert external sound files to the "position, delta" format within SuperCollider.

this tool aims to eliminate this pain point. 


## multisampling 

as a sort of bonus, i've added a basic "multi-sampling" feature. this produces multiple copies of the waveform with successively higher frequencies removed, making them suitable for use in a band-limited wavetable synth.

nigel redmond has an excellent description of the bandlimited-wavetable method on his [earlevel.com]() blog. 

## building

requires `libsndfile`, cmake, and a c++ compiler supporting c++17.

build using cmake. the canonical steps for an out-of-tree build apply:

```
mkdir build
cd build
cmake .. -DCMAKE_RELEASE_TYPE=Release
cmake --build .
```

this will produce the `wttool` executable.

tested with linux and macOS.

## usage

`wttool <options>`

this will process one input file to one or more output files.

`wttool` accepts a number of arguments. each can take a short form or a long form, and all have defaults:

- `-i`, `--input` : path to input file (default `input.wav`)
- `-o`, `--output` : path to output file (defult `output.wav`)
- `-n`, `--length` : output length in samples (default 0 == same as input)
- `-t`, `--interpolation` : sets the interpolation order. "0" is sample-and-hold, "1" is linear, "2" is cubic. (sinc and others not yet implemented.)
- `-p`, `--poweroftwo` : flag; if set, `wttool` will round the output frame count up to the next power of two. (default: false)
- `-s`, `--supercollider` : flag; if set, `wttool` will convert the output to SuperCollider's "position, delta" format. (incidentally doubling the frame count.)
- `-z`, `--zero` : flag; if set, will trim start and end points of output file to the first/last "zero crossing" (defined here as the first sample that is within (2x float-epsilon) of zero.)
- `-m`, `--multi_count` : number of multi-sampled variants to create. a suffix is 
- `-v`, `--multi_interval` : multi-sampling interval, in 

## examples

`wttool --input AKWF_sinharm_0001.wav --output AKWF_sinharm_0001_sc.wav --interpolation 1 --poweroftwo --supercollider`

the [AKWF collection](https://www.adventurekid.se/akrt/waveforms/adventure-kid-waveforms/) is a well-known library of single-cycle waveforms. unfortunately they are all 600 samples long, so it has historically been painful to use them as sources for SuperCollider synths. this command converts one of the AKWF files to the next power of two (1024), using linear interpolation, and converts the output to SuperCollider format, so it can be loaded directly to a `Buffer` for immediate use with `Osc` or `VOsc`.

## other notes

- output format is currently always the same as input format. (e.g. WAV, RAW).
- all formats supported by `libsndfile` should work. (basically everything but mp3, which requires a license.)
- linear interpolation is the default, because cubic interpolation can cause overshoot.
- if overshoot does occur, output should be clamped to [-1, 1] rather than wrapping.

## contributing

- contributions (github PRs) and bug reports (github issues) are welcome!