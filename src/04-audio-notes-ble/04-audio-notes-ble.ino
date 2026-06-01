// 04-audio-notes-ble — BLE UART variation of 04-audio-notes (experiment).
// Identical one-octave note mapping and I2S synth, but the letter arrives over
// Bluetooth LE instead of the USB serial port — so you can play notes from a
// phone (e.g. nRF Connect / LightBlue on iOS) with no cable.
//
// Why BLE, not a "Serial Bluetooth Terminal": the ESP32-S3 has BLE only — no
// classic Bluetooth (SPP) — and iOS has no USB-serial support at all, so a BLE
// UART service is the practical way to send characters from an iPhone.
// See docs/development.md.
//
// Uses the Nordic UART Service (NUS) UUIDs with the core's bundled BLE library
// (no library to install). A lighter alternative is NimBLE-Arduino.
//
// To play: open a BLE terminal app, connect to "Groovebox Notes", and write a
// letter to the RX characteristic. Keys: a w s e d f t g y h u j k
//
// Pins (docs/wiring.md): BCK=5, DIN=6, LRCK/WS=7 — same order as the PCM5102 header
// (BCK·DIN·LCK), so the wires run straight across. SCK tied to GND (internal PLL).

#include <ESP_I2S.h>
#include <math.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Nordic UART Service — the de-facto standard for "serial over BLE".
#define NUS_SERVICE  "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define NUS_RX_CHAR  "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // phone -> ESP32 (write)
#define NUS_TX_CHAR  "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // ESP32 -> phone (notify)

constexpr int PIN_BCLK = 5;   // BCK
constexpr int PIN_DOUT = 6;   // DIN
constexpr int PIN_LRCK = 7;   // LRCK / WS

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr int16_t  AMPLITUDE   = 8000;
constexpr uint32_t NOTE_MS     = 350;

I2SClass i2s;

// --- note mapping + synth: identical to 04-audio-notes ---

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

void playTone(float freq, uint32_t ms) {
  const uint32_t totalFrames = SAMPLE_RATE * ms / 1000;
  const uint32_t fadeFrames  = SAMPLE_RATE * 5 / 1000;
  const float    phaseInc    = 2.0f * PI * freq / SAMPLE_RATE;
  float phase = 0.0f;

  for (uint32_t n = 0; n < totalFrames; n++) {
    float gain = 1.0f;
    if (n < fadeFrames)                     gain = (float)n / fadeFrames;
    else if (n > totalFrames - fadeFrames)  gain = (float)(totalFrames - n) / fadeFrames;

    int16_t s = (int16_t)(AMPLITUDE * gain * sinf(phase));
    phase += phaseInc;
    if (phase >= 2.0f * PI) phase -= 2.0f * PI;

    int16_t frame[2] = { s, s };
    i2s.write((uint8_t *)frame, sizeof(frame));
  }
}

// --- BLE input ---
// The BLE write callback runs on the BLE task. playTone() blocks for NOTE_MS, so
// we must NOT play inside the callback (it would stall the BLE stack). Instead
// the callback enqueues incoming letters and loop() dequeues and plays them.
volatile char    keyQueue[16];
volatile uint8_t qHead = 0, qTail = 0;

void enqueueKey(char c) {
  uint8_t next = (qHead + 1) % sizeof(keyQueue);
  if (next != qTail) {            // drop the key if the queue is full
    keyQueue[qHead] = c;
    qHead = next;
  }
}

bool dequeueKey(char &c) {
  if (qTail == qHead) return false;
  c = keyQueue[qTail];
  qTail = (qTail + 1) % sizeof(keyQueue);
  return true;
}

class RxCallback : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *chr) override {
    String v = chr->getValue();              // arduino-esp32 3.x returns String
    for (size_t i = 0; i < v.length(); i++) enqueueKey(v[i]);
  }
};

void setup() {
  Serial.begin(115200);
  delay(500);

  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!");
    while (true) delay(1000);
  }

  BLEDevice::init("Groovebox Notes");
  BLEServer  *server = BLEDevice::createServer();
  BLEService *svc    = server->createService(NUS_SERVICE);

  BLECharacteristic *rx = svc->createCharacteristic(NUS_RX_CHAR, BLECharacteristic::PROPERTY_WRITE);
  rx->setCallbacks(new RxCallback());

  // TX (notify) isn't used here, but standard NUS terminal apps expect it to exist.
  BLECharacteristic *tx = svc->createCharacteristic(NUS_TX_CHAR, BLECharacteristic::PROPERTY_NOTIFY);
  tx->addDescriptor(new BLE2902());

  svc->start();
  server->getAdvertising()->addServiceUUID(NUS_SERVICE);
  server->getAdvertising()->start();

  Serial.println("\nBLE UART ready — connect to 'Groovebox Notes', write: a w s e d f t g y h u j k");
}

void loop() {
  char c;
  if (!dequeueKey(c)) { delay(2); return; }
  c = tolower(c);
  if (c == '\n' || c == '\r') return;

  float freq = noteForKey(c);
  if (freq == 0.0f) {
    Serial.printf("? '%c'\n", c);
    return;
  }
  Serial.printf("note: %c (%.2f Hz)\n", c, freq);
  playTone(freq, NOTE_MS);
}
