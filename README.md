# TunerScope

- [TunerScope](#tunerscope)
  - [Descriptiom](#descriptiom)
    - [Features](#features)
    - [More usage examples](#more-usage-examples)
    - [Example Use Cases](#example-use-cases)
  - [Settings](#settings)
  - [Technical details](#technical-details)
    - [Compilation from sources](#compilation-from-sources)
    - [Pitch detection algorithm](#pitch-detection-algorithm)


## Descriptiom
TunerScope is an application that visualizes the audio spectrum and precisely detects the musical note, showing pitch deviation in cents.

### Features
- Real-time spectrum display
- High-accuracy note detection
- Flexible configuration (FFT size, sample rate, sample format, etc.)

<img width="680" height="445" alt="main" src="https://github.com/user-attachments/assets/ceb4c769-72ae-4d1c-96e7-ac748746d831" />
<br><br>
While there are many tuners available (especially for guitar), very few provide spectrum visualization.
TunerScope offers an easy and convenient way to analyze the harmonics of a guitar or pickup without the need for a full DAW.

### More usage examples
- A precise tuner
- A vocal pitch training tool with instant feedback
- A spectrum analyzer for any input source: microphone, audio interface, loopback, etc.

### Example Use Cases
- Instrument comparison: a dull-sounding guitar shows fewer harmonics; old strings and cheap pickups are immediately visible
- Dead spot testing: quickly identify problematic frets on an instrument
- Vocal training: sing and instantly check how accurately you’re hitting the pitch
- Instrument tuning: precise tuner with visual deviation in cents

## Settings

The settings window allows you to:
- Select an audio input device
- Configure processing parameters (FFT size, sample rate, sample format, etc.)
<img width="680" height="445" alt="settings" src="https://github.com/user-attachments/assets/99f14328-d58b-46f1-81b8-295105505b53" />
<br>

## Technical details
### Compilation from sources
The code is written in C++ with QML (Qt 6.9). It contains no platform-dependent parts, so it can be built on any platform supported by Qt: Linux, macOS, or Android.
Currently, only prebuilt binaries for Windows are provided, but builds for other platforms might be available for download later.

The project uses the `fftw` library (https://www.fftw.org/). Its sources are included in the project tree for convenience, so you need to build it first. Since the library is not intended to be built directly from GitHub sources, I didn’t use Git submodules or similar approaches.

Typical compilation steps:
```bash
$ mkdir build
$ cd build
$ cmake .. -DCMAKE_BUILD_TYPE=Release
```
... then build with your preferred toolchain.

### Pitch detection algorithm
`TunerScope` uses the FastYin pitch detection algorithm.
Initially, I adapted a Java implementation of this algorithm to C++ with some success. To validate accuracy, I also implemented the "traditional" Yin algorithm (without FFT) and found that my adaptation produced slightly different results - though the differences were very subtle.

To resolve this, I wrote my own `difference()` function using an FFT size that is a power of two. With this, the results exactly matched those of the original Yin algorithm. Both implementations are available in the source code, and switching between them is straightforward.

Note: FastYin is named that for a reason - in testing, it showed at least 5x better performance compared to the traditional Yin implementation.
