// 05-audio-pitch-wave — the pitch glides up and down continuously, following a sine
// contour, so the note rises and falls progressively instead of stepping between
// discrete notes. Plays automatically, no input needed.
//
// What this is: pitch modulation by a low-frequency oscillator (LFO). A second, slow
// sine (the LFO) sweeps the frequency of the audible sine up and down. Slow + wide =
// a "siren"; fast + narrow would be "vibrato". The continuous pitch glide is a glissando.
//
// Builds on 03-PCM5102A-test: same I2S sine, but the frequency now changes every sample.
//
// Pins (docs/hardware/wiring.md): BCK=5, DIN=6, LRCK/WS=7 — same order as the PCM5102 header.
// SCK tied to GND (internal PLL). See docs/hardware/modules/pcm5102.md.

#include <ESP_I2S.h>
#include <math.h>

constexpr int PIN_BCLK = 5;   // BCK
constexpr int PIN_DOUT = 6;   // DIN
constexpr int PIN_LRCK = 7;   // LRCK / WS

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr int16_t  AMPLITUDE   = 8000;    // < 32767, leave headroom

// Sweep settings — tweak these to taste:
constexpr float FREQ_LOW  = 220.0f;   // bottom of the glide (A3)
constexpr float FREQ_HIGH = 880.0f;   // top of the glide    (A5)
constexpr float SWEEP_HZ  = 0.5f;     // up-and-down cycles per second (0.5 = 2 s per round trip)

I2SClass i2s;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nPitch wave — frequency glides up and down (sine LFO)");

  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!");
    while (true) delay(1000);
  }
  Serial.println("I2S ready — you should hear a rising/falling tone");
}

void loop() {
  // Two oscillators: `lfoPhase` is the slow sweep that shapes the pitch; `phase` is the
  // audible sine. Both kept static so they stay continuous across loop() calls.
  static float lfoPhase = 0.0f;
  static float phase    = 0.0f;

  const float lfoInc = 2.0f * PI * SWEEP_HZ / SAMPLE_RATE;
  const float center = (FREQ_HIGH + FREQ_LOW) * 0.5f;
  const float depth  = (FREQ_HIGH - FREQ_LOW) * 0.5f;

  int16_t frame[2];
  for (int i = 0; i < 256; i++) {
    // Current pitch = center frequency + a sine wobble. As lfoPhase walks 0→2π the
    // frequency smoothly rises to FREQ_HIGH and falls back to FREQ_LOW.
    float freq = center + depth * sinf(lfoPhase);
    lfoPhase += lfoInc;
    if (lfoPhase >= 2.0f * PI) lfoPhase -= 2.0f * PI;

    // Advance the audible oscillator at the (now changing) pitch.
    phase += 2.0f * PI * freq / SAMPLE_RATE;
    if (phase >= 2.0f * PI) phase -= 2.0f * PI;

    int16_t s = (int16_t)(AMPLITUDE * sinf(phase));
    frame[0] = s;  // left
    frame[1] = s;  // right
    i2s.write((uint8_t *)frame, sizeof(frame));
  }
}
