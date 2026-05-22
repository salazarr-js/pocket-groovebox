# Portable Music Instrument Project — Complete Summary

## Origin

The project started after receiving a **Stylophone Beat** as a gift from Renzo.

The device sparked interest in:

* Portable music devices
* Standalone instruments
* Tactile interfaces
* Embedded systems
* Music hardware
* Creative coding
* Realtime interaction
* Web-native instruments

Key realization:

> Small playful devices can inspire surprisingly deep creative experiences.

---

# Long-Term Vision

Create a family of:

* Portable music machines
* Standalone synthesizers
* Pocket grooveboxes
* Creative music toys
* Hybrid hardware/software instruments
* Web-native music experiences

The goal is NOT to build:

* Another DAW
* Another workstation
* Another feature-heavy synthesizer

The goal IS to build:

* Instant fun
* Tactile interaction
* Playful experimentation
* Minimal interfaces
* Expressive hardware
* Portable creativity

---

# Major Inspirations

## Stylophone Beat

Inspiration:

* Simple interaction
* Immediate feedback
* Portable workflow
* Drum sequencing
* Looping

Ideas:

* MIDI expansion
* ESP32 integration
* Standalone groovebox concepts

---

## Teenage Engineering PO-33 K.O!

Inspiration:

* Pocket-sized form factor
* Sampling workflow
* Creative constraints
* Minimal UI
* Fast experimentation

Lessons:

* Small devices can be extremely expressive
* Limitations can improve creativity

---

## Teenage Engineering OP-1

Major inspiration source.

Characteristics:

* Portable music workstation
* Synthesizer
* Sampler
* Tape workflow
* Playful operating system
* Strong industrial design

Key realization:

The OP-1 succeeds because of:

* Interaction design
* Visual design
* Constraints
* Personality
* Feedback

Not because of feature count.

---

## Korg nanoKEY2

Inspiration:

* Compact keyboard
* Minimal interaction
* Portable form factor
* Simple MIDI workflow

Desired keyboard style for the project:

* One octave
* Mini keys
* Similar proportions to nanoKEY2

---

## Monogram Duo

Inspiration:

* Premium controls
* Knobs as primary interaction
* Modular hardware
* Custom workflows

Lessons:

* Physical controls are part of the experience
* Hardware should feel intentional

---

# Product Concept

Current concept:

A portable standalone synthesizer / sampler / loop station inspired by:

* OP-1
* PO-33
* nanoKEY2
* Stylophone Beat

Characteristics:

* Small
* Lightweight
* Battery powered
* Speaker included
* OLED display
* One octave keyboard
* Standalone operation

Not dependent on a computer.

---

# Keyboard Design

Current direction:

## One octave keyboard

Keys:

```txt
C D E F G A B
C D E F G A B
C
```

Total:

```txt
13 keys
```

Configuration:

* 8 white keys
* 5 black keys

Similar to:

* nanoKEY2
* mini MIDI keyboards
* compact synth layouts

---

# Hardware Platform Decisions

## Current available hardware

Existing board:

### NodeMCU ESP8266 (ESP-12E)

Capabilities:

* WiFi
* GPIO
* Buttons
* Encoders
* OLED
* WebSocket communication

Good for:

* Rapid prototyping
* Input handling
* Browser-based synths

Not ideal for:

* Advanced standalone audio
* Bluetooth MIDI
* Complex DSP

---

## Future hardware

Recommended upgrade:

### ESP32-S3

Reasons:

* More RAM
* Faster CPU
* Bluetooth
* BLE MIDI
* Native USB
* Better audio support
* Better for standalone instruments

---

# ESP8266 Prototype Phase

Recommended architecture:

```txt
Keyboard
      ↓
ESP8266
      ↓ WiFi
Browser
      ↓
Tone.js
      ↓
Audio
```

Purpose:

Validate:

* Interaction
* Layout
* Workflow
* UI
* Sequencing

Before building standalone hardware.

---

# ESP32 Standalone Architecture

Target architecture:

```txt
Keys
Encoders
Buttons
      ↓
ESP32-S3
      ↓
I2S DAC / Amplifier
      ↓
Speaker / Headphones
```

Capabilities:

* Synth waves
* Loops
* Sequencing
* Playback
* Standalone operation

---

# ESP32 vs Raspberry Pi

## ESP32

Suitable for:

* Simple synths
* Drum machines
* Sequencers
* Loop stations
* MIDI devices
* Battery-powered instruments

Pros:

* Fast boot
* Low power
* Small size
* Low cost

Cons:

* Limited memory
* Limited DSP
* Limited sample storage

---

## Raspberry Pi

Suitable for:

* Large sample libraries
* Advanced DSP
* Effects
* Workstation-level audio
* Complex synthesis

Pros:

* Powerful
* Linux ecosystem
* Massive storage

Cons:

* Larger
* Higher power consumption
* Slower boot
* More complexity

Conclusion:

For Version 1:

```txt
ESP32-S3
```

is preferred.

---

# Audio System

Recommended module:

## MAX98357A

Benefits:

* I2S audio
* Small footprint
* Direct speaker support
* Easy ESP32 integration

Architecture:

```txt
ESP32
      ↓
MAX98357A
      ↓
Speaker
```

---

# Display

Recommended:

## OLED SSD1306

Size:

```txt
0.96 inch
```

Benefits:

* Low power
* Small
* Cheap
* Clean aesthetic
* Easy integration

Used for:

* BPM
* Presets
* Waveforms
* Menus

---

# Controls

Recommended:

## Rotary Encoders

Model:

### EC11

Quantity:

```txt
2 encoders
```

Uses:

* Volume
* Navigation
* BPM
* Sound selection
* Parameters

---

# Power System

## Battery Options

### 18650 Li-Ion

Recommended for Version 1.

Advantages:

* Safe
* Replaceable
* Cheap
* High capacity
* Easy sourcing

---

### LiPo

Recommended for future compact versions.

Advantages:

* Thin
* Lightweight
* Better industrial design

Disadvantages:

* More fragile
* More difficult to manage

---

# Battery Count Discussion

Conclusion:

Use:

```txt
1 battery
```

instead of two.

Reason:

Devices typically use:

```txt
Battery
      ↓
Regulator
      ↓
5V / 3.3V rails
```

Benefits:

* Simpler
* Safer
* Smaller
* Lower cost

---

# Power Architecture

Example:

```txt
18650
      ↓
USB-C charging module
      ↓
Boost converter
      ↓
5V
      ↓
ESP32 regulator
      ↓
3.3V
```

---

# Storage

Current recommendation:

No microSD initially.

Start with:

* Internal flash
* Built-in sounds
* Built-in synth engine

Add microSD later if needed for:

* Samples
* User content
* Recording
* Expansion

---

# PCB Design

Recommended software:

## KiCad

Reasons:

* Free
* Open source
* Professional
* Huge community

Future PCB:

```txt
ESP32-S3
+
USB-C
+
Battery charger
+
OLED
+
Speaker amp
+
Encoders
+
Keyboard matrix
```

---

# Proposed MVP

## Pocket Groove Prototype V1

Features:

* 13-key keyboard
* ESP32-S3
* OLED display
* 2 rotary encoders
* Speaker
* Headphone output
* USB-C charging
* Internal synth
* Simple loop station
* Step sequencer
* Battery powered

No:

* Advanced effects
* Massive sampling
* Complex DAW features

Focus:

```txt
Small
Simple
Portable
Fun
```

---

# Design Philosophy

## Principle 1

Instant fun.

The instrument should:

* Boot quickly
* Respond immediately
* Encourage experimentation

---

## Principle 2

Playful minimalism.

Fewer controls.

Better interaction.

---

## Principle 3

Standalone first.

The device should work:

* Anywhere
* Without a computer
* Without internet

---

## Principle 4

Future hybrid experience.

Optional companion app:

* Web MIDI
* Tone.js
* Three.js visuals
* Pattern editor
* Cloud sync
* Sharing

---

# Final Direction

The most promising direction is:

> A small standalone ESP32-S3 powered synthesizer / sampler / loop station with a one-octave mini keyboard, OLED display, speaker, rechargeable battery, and strong emphasis on tactile interaction, portability, simplicity, and fun.

Inspired by:

* Stylophone Beat
* Teenage Engineering PO-33
* Teenage Engineering OP-1
* Korg nanoKEY2
* Monogram Duo
* Creative coding culture
* Embedded systems
* Web-native music tools
