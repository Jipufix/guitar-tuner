# Guitar Tuner Using FFT Algorithm

## Overview

This project is a real-time guitar tuner implemented in C. It uses a Fast Fourier Transform (FFT) algorithm to analyze audio signals and identify their dominant frequencies. The detected frequencies are then mapped to musical notes on the Western Musical Scale, making it suitable for tuning guitars and other instruments.

## Features

- **Real-Time Frequency Analysis**: Processes audio signals in real-time using FFT.
- **Note Detection**: Converts frequencies to corresponding musical notes (e.g., A4, E2).
- **Wide Frequency Range**: Supports frequencies from C2 (~65 Hz) to B8 (~8 kHz).
- **Customizable Sampling Rate**: Adjustable sampling rates for different audio setups.

## Requirements

- **Hardware**: STM32F7 Discovery Board (or similar DSP-capable microcontroller).
- **Libraries**: ARM CMSIS DSP library for FFT and other DSP operations.
- **Audio Source**: Compatible with stereo audio input.

## How It Works

1. Audio signals are captured and converted from stereo to mono.
2. The FFT algorithm analyzes the signal to identify dominant frequencies.
3. Frequencies are mapped to musical notes using logarithmic calculations.
4. Results are displayed as text indicating the detected note.

## Limitations

- Low-frequency distortion may occur below C2 (~65 Hz). Lowering the input volume can mitigate this issue.
- High-frequency irrelevant signals above 4 kHz are filtered out.

## Installation and Usage

1. Clone this repository:
      git clone https://github.com/yourusername/guitar-tuner.git
2. Build the project using your preferred IDE or toolchain for STM32 development.
3. Flash the binary onto your STM32F7 Discovery Board.
4. Connect an audio source (e.g., guitar, microphone) and observe the detected notes on the display.

## Code Structure

- `myAlgorithm.c`: Core implementation of the FFT-based guitar tuner.
- `myAlgorithm.h`: Header file defining function prototypes and constants.
- Dependencies:
- `filters.h`: Functions for audio signal conversion.
- `processAudio.h`: Audio processing utilities.
- `arm_math.h`: ARM CMSIS DSP library.

## References

- [FFT Documentation](https://arm-software.github.io/CMSIS_5/DSP/html/arm_fft_bin_example_f32_8c-example.html#a6)
- [Tone Generator](https://www.szynalski.com/tone-generator)
- [Musical Scale Formulas](https://newt.phys.unsw.edu.au/jw/notes.html)
