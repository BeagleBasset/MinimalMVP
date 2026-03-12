# MinimalMVP

A minimal, real-time spectrogram visualizer written in C/C++ that shows audio frequencies on a logarithmic scale with adaptive contrast.

<img width="1370" height="946" alt="kép" src="https://github.com/BeagleBasset/MinimalMVP/blob/main/pic.jpg" />


## Features

- Real-time FFT processing of audio input
- Logarithmic frequency scale for natural audio perception

## Dependencies

- C/C++ compiler (GCC/Clang)
- [miniaudio](https://github.com/mackron/miniaudio)

## Installation

```bash
git clone https://github.com/BeagleBasset/MinimalMVP.git
```
```bash
cd MinimalMVP
```
```bash
make 
```

## Usage
```bash
./player path/to/audiofile.wav
```
## Controls
```q```     → Quit

```Space``` → Start/Stop
