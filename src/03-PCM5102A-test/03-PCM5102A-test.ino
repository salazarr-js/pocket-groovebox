// PCM5102 tone test — ESP32-S3 + PCM5102 I2S DAC.
// Plays a continuous 440 Hz sine tone to confirm the I2S audio path works.
//
// Library: ESP_I2S — bundled with the Arduino-ESP32 core (3.x), NOT the generic
// Arduino "I2S.h". The generic library doesn't target the ESP32-S3; ESP_I2S is the
// core's own driver and lets us pick the pins. See docs/hardware/modules/pcm5102.md.
//
// Pins (see docs/hardware/wiring.md): BCK=5, DIN=6, LRCK/WS=7 — same order as the PCM5102 header
// (BCK·DIN·LCK), so the wires run straight across. PCM5102 needs no MCLK — its SCK pad is
// tied to GND so the chip uses its internal PLL.

#include <ESP_I2S.h>
#include <math.h>

constexpr int PIN_BCLK = 5;   // BCK  — bit clock
constexpr int PIN_DOUT = 6;   // DIN  — serial data to the DAC
constexpr int PIN_LRCK = 7;   // LRCK / WS — word select

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr float    TONE_HZ     = 440.0f;  // A4
constexpr int16_t  AMPLITUDE   = 8000;    // < 32767, leave headroom

I2SClass i2s;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nPCM5102 tone test — 440 Hz");

  // setPins(bclk, ws, dout, din, mclk). No MCLK / no input → -1.
  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);

  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!");
    while (true) delay(1000);
  }
  Serial.println("I2S ready — you should hear a tone");
}

void loop() {
  static float phase = 0.0f;
  const float phaseInc = 2.0f * PI * TONE_HZ / SAMPLE_RATE;

  // Write a block of stereo frames; i2s.write blocks on the DMA, pacing playback.
  int16_t frame[2];
  for (int i = 0; i < 256; i++) {
    int16_t s = (int16_t)(AMPLITUDE * sinf(phase));
    phase += phaseInc;
    if (phase >= 2.0f * PI) phase -= 2.0f * PI;
    frame[0] = s;  // left
    frame[1] = s;  // right
    i2s.write((uint8_t *)frame, sizeof(frame));
  }
}
