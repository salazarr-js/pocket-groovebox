// PAM8403 + speaker test — ESP32-S3 + PCM5102 I2S DAC → PAM8403 class-D amp → speaker.
//
// Separate from src/03-PCM5102A-test (which verifies the DAC line out / headphone path at
// full headroom). THIS sketch is for the amp + speaker leg: the PAM8403 (QA03 mini) has
// fixed gain (~24 dB) and no volume pot, so a small speaker overdrives easily. It plays a
// 440 Hz tone while stepping the digital amplitude from quiet to loud, printing each level
// over serial — listen for where it starts to distort and pick a safe AMPLITUDE for your
// speaker. See docs/hardware/modules/pam8403.md and docs/hardware/wiring.md.
//
// Pins (see docs/hardware/wiring.md): BCK=5, DIN=6, LRCK/WS=7. PCM5102 SCK tied to GND (internal PLL).
// Amp input: PCM5102 LROUT/ROUT/AGND → amp L/R/GND. Speaker across L+/L− (BTL — never to GND).

#include <ESP_I2S.h>
#include <math.h>

constexpr int PIN_BCLK = 5;   // BCK  — bit clock
constexpr int PIN_DOUT = 6;   // DIN  — serial data to the DAC
constexpr int PIN_LRCK = 7;   // LRCK / WS — word select

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr float    TONE_HZ     = 440.0f;  // A4

// Measured comfortable level on our 4 Ohm / 3 W / 30 mm speaker: AMPLITUDE ~= 320.
// If ALL levels sound equally loud, the DAC AGND -> amp input GND wire is missing — the amp
// is amplifying noise, not the signal (see docs/hardware/modules/pam8403.md notes). A constant low
// hiss at quiet levels is the amp's normal noise floor.

// Amplitude levels to sweep, quiet → loud (full scale is 32767). A tiny speaker behind the
// PAM8403's fixed ~24 dB gain overdrives almost immediately, so start near-silent and
// double each step (~6 dB) to find where it stays clean. If even the lowest step is loud
// and distorted, the amp gain is simply too high for this speaker — see the notes below
// and docs/hardware/modules/pam8403.md (attenuate the input or use a bigger speaker).
constexpr int16_t  LEVELS[]   = {20, 40, 80, 160, 320, 640, 1280, 2560};
constexpr int      NUM_LEVELS = sizeof(LEVELS) / sizeof(LEVELS[0]);
constexpr uint32_t HOLD_MS    = 2000;     // tone time at each level
constexpr uint32_t GAP_MS     = 500;      // silence between levels, so each step is distinct

I2SClass i2s;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nPAM8403 + speaker test — 440 Hz, amplitude sweep");
  Serial.println("Listen for where it starts to distort; that's your ceiling.");

  // setPins(bclk, ws, dout, din, mclk). No MCLK / no input → -1.
  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);

  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!");
    while (true) delay(1000);
  }
  Serial.println("I2S ready — sweeping levels");
}

void loop() {
  static float phase = 0.0f;
  const float phaseInc = TWO_PI * TONE_HZ / SAMPLE_RATE;

  for (int lvl = 0; lvl < NUM_LEVELS; lvl++) {
    const int16_t amp = LEVELS[lvl];
    // Approx level in dB relative to full scale (32767), for reference.
    const float dbfs = 20.0f * log10f((float)amp / 32767.0f);
    Serial.printf("level %d/%d  AMPLITUDE=%d  (~%.0f dBFS)\n", lvl + 1, NUM_LEVELS, amp, dbfs);

    // Play this level for HOLD_MS. i2s.write blocks on the DMA, pacing playback.
    const uint32_t framesToPlay = SAMPLE_RATE * HOLD_MS / 1000;
    int16_t frame[2];
    for (uint32_t n = 0; n < framesToPlay; n++) {
      int16_t s = (int16_t)(amp * sinf(phase));
      phase += phaseInc;
      if (phase >= TWO_PI) phase -= TWO_PI;
      frame[0] = s;  // left
      frame[1] = s;  // right
      i2s.write((uint8_t *)frame, sizeof(frame));
    }

    // Silence gap so each step is clearly separate (not heard as a melody).
    const uint32_t gapFrames = SAMPLE_RATE * GAP_MS / 1000;
    int16_t silent[2] = {0, 0};
    for (uint32_t n = 0; n < gapFrames; n++) i2s.write((uint8_t *)silent, sizeof(silent));
  }
  Serial.println("— sweep done, repeating —");
}
