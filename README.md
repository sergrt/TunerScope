# TunerScope

- [TunerScope](#tunerscope)
  - [Descriptiom](#descriptiom)
    - [Features](#features)
    - [More usage examples](#more-usage-examples)
    - [Example Use Cases](#example-use-cases)
  - [Settings](#settings)
  - [Technical details](#technical-details)
    - [Brief notes](#brief-notes)
    - [Pitch detection](#pitch-detection)


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
### Brief notes
Code is written in C++ with QML (Qt 6.9). It has no platform-dependent parts, so can be compiled on any platform subborting Qt. Currently I provide only Windows prebuild binaries, more platforms will be available for downloading later.
### Pitch detection
`TunerScope` uses FastYin pitch detection algorithm. I have tried to adapt java implementation of this algorithm to C++ with some success, but decided to add "traditional" Yin algorithm (without fft) to check te accuracy, and found that my adaptation produces slightly different results (difference is really subtle). So I decided to implement my own `difference()` function implementation, with fft size of power of 2, and results are exactly as with Yin algorithm. Both implementations are available in source code, as long aas Yin implementation.
