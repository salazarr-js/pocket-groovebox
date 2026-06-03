// 04-audio-notes — type a letter in the Serial Monitor to play a note.
// Maps a QWERTY "piano" row to one octave (C4–C5) and plays each key as a short
// sine tone through the PCM5102 over I2S. Combines the serial-input pattern from
// 02-led-serial with the I2S tone from 03-PCM5102A-test.
//
// Keyboard layout (one octave, 13 keys = the instrument's 8 white + 5 black):
//   white:  a s d f g h j k   ->  C  D  E  F  G  A  B  C
//   black:  w e   t y u       ->  C# D#   F# G# A#
//
// Pins (docs/hardware/wiring.md): BCK=5, DIN=6, LRCK/WS=7 — same order as the PCM5102 header
// (BCK·DIN·LCK), so the wires run straight across. PCM5102 needs no MCLK — its SCK pad is
// tied to GND so the chip uses its internal PLL. See docs/hardware/modules/pcm5102.md.

#include <ESP_I2S.h>
#include <math.h>

constexpr int PIN_BCLK = 5;   // BCK  — bit clock
constexpr int PIN_DOUT = 6;   // DIN  — serial data to the DAC
constexpr int PIN_LRCK = 7;   // LRCK / WS — word select

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr int16_t  AMPLITUDE   = 8000;   // < 32767, leave headroom
constexpr uint32_t NOTE_MS     = 350;    // how long each key sounds

I2SClass i2s;

// Frequency for a mapped letter, or 0 if the key isn't part of the keyboard.
float noteForKey(char c) {
  switch (c) {
    case 'a': return 261.63f;  // C4
    case 'w': return 277.18f;  // C#4
    case 's': return 293.66f;  // D4
    case 'e': return 311.13f;  // D#4
    case 'd': return 329.63f;  // E4
    case 'f': return 349.23f;  // F4
    case 't': return 369.99f;  // F#4
    case 'g': return 392.00f;  // G4
    case 'y': return 415.30f;  // G#4
    case 'h': return 440.00f;  // A4
    case 'u': return 466.16f;  // A#4
    case 'j': return 493.88f;  // B4
    case 'k': return 523.25f;  // C5
    default:  return 0.0f;
  }
}

// Play a sine tone of `freq` Hz for `ms` milliseconds. The short fade in/out
// avoids the click you'd get from starting or stopping at a non-zero sample.
void playTone(float freq, uint32_t ms) {
  const uint32_t totalFrames = SAMPLE_RATE * ms / 1000;
  const uint32_t fadeFrames  = SAMPLE_RATE * 5 / 1000;   // 5 ms ramps
  const float    phaseInc    = 2.0f * PI * freq / SAMPLE_RATE;
  float phase = 0.0f;

  for (uint32_t n = 0; n < totalFrames; n++) {
    float gain = 1.0f;
    if (n < fadeFrames)                     gain = (float)n / fadeFrames;
    else if (n > totalFrames - fadeFrames)  gain = (float)(totalFrames - n) / fadeFrames;

    int16_t s = (int16_t)(AMPLITUDE * gain * sinf(phase));
    phase += phaseInc;
    if (phase >= 2.0f * PI) phase -= 2.0f * PI;

    int16_t frame[2] = { s, s };  // left, right
    i2s.write((uint8_t *)frame, sizeof(frame));
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // setPins(bclk, ws, dout, din, mclk). No MCLK / no input → -1.
  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!");
    while (true) delay(1000);
  }
  Serial.println("\nAudio notes — play with: a w s e d f t g y h u j k");
}

void loop() {
  if (!Serial.available()) return;
  char c = tolower(Serial.read());            // accept upper- or lowercase
  if (c == '\n' || c == '\r') return;         // ignore line endings

  float freq = noteForKey(c);
  if (freq == 0.0f) {
    Serial.printf("? '%c' — keys: a w s e d f t g y h u j k\n", c);
    return;
  }
  Serial.printf("note: %c (%.2f Hz)\n", c, freq);
  playTone(freq, NOTE_MS);
}
