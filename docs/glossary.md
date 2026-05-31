# Pocket Groovebox — Glossary

Short definitions for the acronyms and terms used across the project docs.

## Music & instrument concepts

- **Groovebox** — a self-contained instrument that combines sound generation, sequencing, and pattern-based performance in one box.
- **Synth (synthesizer)** — generates sound electronically from waveforms rather than recordings.
- **Sampler** — plays back and manipulates recorded audio snippets (samples).
- **Loop station** — records a phrase and repeats it, letting you layer parts live.
- **Sequencer / step sequencer** — programs notes/drum hits into steps that play back in a loop.
- **DAW (Digital Audio Workstation)** — full-featured music production software (e.g. Ableton, Logic). Explicitly *not* what this project aims to be.
- **BPM (Beats Per Minute)** — tempo of the sequencer/loops.
- **Waveform** — the shape of an oscillator's output (sine, square, saw, etc.), defining its basic tone.
- **Preset** — a saved set of sound/parameter settings.

## Protocols & signals

- **MIDI (Musical Instrument Digital Interface)** — standard protocol for sending note/control data between instruments and software.
- **BLE MIDI** — MIDI carried over Bluetooth Low Energy (wireless MIDI).
- **I2S (Inter-IC Sound)** — a digital serial protocol for sending audio between chips (e.g. MCU → audio amp).
- **PWM (Pulse-Width Modulation)** — switching a signal on/off rapidly; can drive simple audio or control brightness/motors.
- **DSP (Digital Signal Processing)** — math performed on audio in real time (filters, effects, synthesis).

## Hardware & components

- **MCU (Microcontroller Unit)** — the small chip that runs the firmware and handles inputs/audio.
- **ESP32-S3** — the project's WiFi/Bluetooth microcontroller, used for both prototyping and V1 (ample RAM/CPU, BLE MIDI, native USB).
- **DAC (Digital-to-Analog Converter)** — turns digital audio into an analog signal a speaker can play.
- **MAX98357A** — a small I2S audio amplifier that drives a speaker directly.
- **Amp (amplifier)** — boosts the audio signal to drive a speaker or headphones.
- **TFT (Thin-Film-Transistor LCD)** — the color LCD panel type used by the project's display.
- **ST7789** — a common SPI display driver chip for small color TFT LCDs (e.g. 170×320).
- **EC11** — a common rotary encoder used for volume, navigation, and parameters.
- **Rotary encoder** — a knob that reports rotation steps (and often a push button).
- **Keyboard matrix** — wiring scheme that reads many keys with fewer GPIO pins.
- **GPIO (General-Purpose Input/Output)** — configurable pins on the MCU for buttons, LEDs, etc.

## Power

- **18650** — a common cylindrical rechargeable Li-Ion battery cell.
- **Li-Ion (Lithium-Ion)** — rechargeable battery chemistry; robust and high-capacity.
- **LiPo (Lithium-Polymer)** — thinner/lighter battery chemistry; more fragile to manage. Considered for future compact versions.
- **Boost converter** — steps a lower battery voltage up to a higher rail (e.g. 5V).
- **Regulator** — provides a stable voltage rail (e.g. 3.3V) for the electronics.

## Tools & storage

- **PCB (Printed Circuit Board)** — the board that holds and connects the components.
- **KiCad** — free, open-source PCB design software.
- **microSD** — removable flash storage card; a possible later addition for samples.
- **Flash** — non-volatile on-board storage for firmware and built-in sounds.
