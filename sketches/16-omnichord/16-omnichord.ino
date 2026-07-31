/*
 * Sketch 16 — Omnichord
 *
 * Chord select (PCF8575, latching) + strum plate (joystick horizontal axis).
 * Sweeping the joystick across its range arpegiates the selected chord with a
 * harp-like pluck decay. Up to N_STRUM notes ring simultaneously.
 *
 * Controls:
 *   PCF keys           — select root note (latching: stays when released)
 *   Joystick L/R       — strum plate; sweep to trigger chord tones
 *   Joystick U/D       — chord type: up=mayor, down=menor, center=dom7
 *   Joystick SW        — cycle volume (8 steps, ~3 dB each)
 *   Joystick SW @ boot — force joystick recalibration
 *
 * Sound layers:
 *   Drone  — root note sostenido (fade-in 80ms) al seleccionar acorde
 *   Strum  — 8 notas del acorde con decay tipo arpa encima del drone
 *
 * Strum note layout (12 positions, low→high, ~3 octaves — igual que el OM-27):
 *   MAJOR  1  3  5  1  3  5  1  3  5  1  3  5   (C3→G6 for C root)
 *   MINOR  1 b3  5  1 b3  5  1 b3  5  1 b3  5
 *   DOM7   1  3  5 b7  1  3  5 b7  1  3  5 b7
 *
 * PCF8575 key layout:
 *   P00=E  P01=F  P02=F#  P03=G  P04=G#  P05=A  P06=A#  P07=B
 *   P10=D# P11=D  P12=C#  P13=C
 *
 * Wiring: same as sketch 15.
 *   I2S  BCK=5 DIN=6 LRCK=7
 *   SPI  SCL=9 SDA=10 RES=11 DC=12 CS=13 BLK=14
 *   I2C  SDA=4 SCL=8
 *   Joy  VRx=2 VRy=1 SW=21
 *
 * Libs: "GFX Library for Arduino" (moononournation), ESP_I2S (bundled with core).
 */

#include <Wire.h>
#include <Arduino_GFX_Library.h>
#include <ESP_I2S.h>
#include <Preferences.h>
#include <math.h>

enum ChordType { MAJOR, MINOR, DOM7 };

// Strum intervals from root (semitones), 12 positions low→high (~3 octaves)
const int STRUM[3][12] = {
  {  0,  4,  7, 12, 16, 19, 24, 28, 31, 36, 40, 43 },  // MAJOR: 4 reps de 1-3-5
  {  0,  3,  7, 12, 15, 19, 24, 27, 31, 36, 39, 43 },  // MINOR: 4 reps de 1-b3-5
  {  0,  4,  7, 10, 12, 16, 19, 22, 24, 28, 31, 34 },  // DOM7:  3 reps de 1-3-5-b7
};

// ─── display ─────────────────────────────────────────────────────────────────
#define PIN_SCLK 9
#define PIN_MOSI 10
#define PIN_RST  11
#define PIN_DC   12
#define PIN_CS   13
#define PIN_BL   14
#define BLACK RGB565_BLACK
#define RED   RGB565_RED
#define TEAL  0x05F7

Arduino_DataBus* bus = new Arduino_ESP32SPI(PIN_DC, PIN_CS, PIN_SCLK, PIN_MOSI, GFX_NOT_DEFINED);
Arduino_GFX*     gfx = new Arduino_ST7789(bus, PIN_RST, 1, true, 170, 320, 35, 0, 35, 0);

// ─── audio ───────────────────────────────────────────────────────────────────
constexpr uint32_t SAMPLE_RATE = 44100;
constexpr int      BLOCK       = 128;
constexpr int      N_STRUM     = 12;  // ~3 octaves, como la placa del Omnichord OM-27

// Harp envelope: near-instant attack, exponential decay (~1 s time constant).
// After 1 s the note is at 37%; after 3 s ≈ 5% — rings out naturally.
const float ATT_STEP    = 1.0f / (SAMPLE_RATE * 0.002f);
const float DECAY_COEFF = expf(-1.0f / (SAMPLE_RATE * 1.0f));

// Drone (root note sostenido): fade-in / fade-out suave al cambiar acorde.
const float DRONE_ATT = 1.0f / (SAMPLE_RATE * 0.08f);
const float DRONE_REL = 1.0f / (SAMPLE_RATE * 0.08f);

I2SClass i2s;

struct Voice {
  float phase     = 0;
  float inc       = 0;
  float env       = 0;
  bool  active    = false;
  bool  attacking = false;
};
Voice voices[N_STRUM];

// Voz drone: raíz sostenida mientras haya acorde seleccionado.
// Cambiar acorde reafina el drone sin resetear fase (sin click).
struct Drone {
  float phase = 0;
  float inc   = 0;
  float env   = 0;
  bool  on    = false;
} drone;

// Volume ceiling measured on the speaker+amp chain (~800 saturates the PAM8403 input).
const float VOL_LEVELS[8] = { 70, 100, 140, 200, 285, 400, 565, 800 };
int   volIdx    = 7;
float masterAmp = VOL_LEVELS[7];

float semiToFreq(int semi) { return 130.81f * powf(2.0f, semi / 12.0f); }

void triggerVoice(int pos, int semi) {
  Voice& v    = voices[pos];
  v.inc       = 2.0f * PI * semiToFreq(semi) / SAMPLE_RATE;
  if (v.env < 0.001f) v.phase = 0.0f;  // solo resetear fase si está en silencio
  v.active    = true;
  v.attacking = true;
  // env no se resetea: si la voz ya suena, el ataque sube desde donde está
}

void audioBlock() {
  int16_t buf[BLOCK * 2];
  for (int i = 0; i < BLOCK; i++) {
    float mix = 0;

    // ── drone: root note sostenido ──────────────────────────────────────────
    float droneTarget = drone.on ? 1.0f : 0.0f;
    if (drone.env < droneTarget) { drone.env += DRONE_ATT; if (drone.env > droneTarget) drone.env = droneTarget; }
    else if (drone.env > droneTarget) { drone.env -= DRONE_REL; if (drone.env < droneTarget) drone.env = droneTarget; }
    if (drone.env > 0) {
      mix += drone.env * sinf(drone.phase) * 0.5f;  // 50%: más suave que el strum
      drone.phase += drone.inc;
      if (drone.phase >= 2.0f * PI) drone.phase -= 2.0f * PI;
    }

    // ── strum: notas del acorde con decay de arpa ───────────────────────────
    for (int v = 0; v < N_STRUM; v++) {
      if (!voices[v].active) continue;
      if (voices[v].attacking) {
        voices[v].env += ATT_STEP;
        if (voices[v].env >= 1.0f) { voices[v].env = 1.0f; voices[v].attacking = false; }
      } else {
        voices[v].env *= DECAY_COEFF;
        if (voices[v].env < 0.0001f) { voices[v].active = false; continue; }
      }
      mix += voices[v].env * sinf(voices[v].phase);
      voices[v].phase += voices[v].inc;
      if (voices[v].phase >= 2.0f * PI) voices[v].phase -= 2.0f * PI;
    }

    // tanh soft clip: drone + strum completo nunca clipean.
    int16_t s = (int16_t)(masterAmp * tanhf(mix));
    buf[i * 2] = s; buf[i * 2 + 1] = s;
  }
  i2s.write((uint8_t*)buf, sizeof(buf));
}

// ─── PCF8575 ─────────────────────────────────────────────────────────────────
#define I2C_SDA  4
#define I2C_SCL  8
#define PCF_ADDR 0x20
#define DEBOUNCE_MS 8

const char* NOTE_NAME[16] = {
  "E","F","F#","G","G#","A","A#","B","D#","D","C#","C",
  nullptr,nullptr,nullptr,nullptr
};

int nameToSemi(const char* n) {
  const char* N[] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
  for (int i = 0; i < 12; i++) if (!strcmp(n, N[i])) return i;
  return -1;
}

uint16_t prevRaw = 0xFFFF, stableState = 0xFFFF;
uint32_t lastChangeMs = 0;

uint16_t readPCF() {
  Wire.requestFrom((uint8_t)PCF_ADDR, (uint8_t)2);
  if (Wire.available() < 2) return 0xFFFF;
  return Wire.read() | ((uint16_t)Wire.read() << 8);
}

// ─── joystick ────────────────────────────────────────────────────────────────
#define VRX_PIN        2
#define VRY_PIN        1
#define JOY_SW         21
#define THRESHOLD_PCT  25
#define CAL_SAMPLES    50
#define SW_DEBOUNCE_MS 50

Preferences prefs;
int centerX, centerY, threshold;

void computeThreshold() {
  int minRange = min({centerX, centerY, 4095 - centerX, 4095 - centerY});
  threshold = (minRange * THRESHOLD_PCT) / 100;
}

void runCalibration() {
  Serial.println("Calibrando — no tocar joystick...");
  delay(800);
  long sx = 0, sy = 0;
  for (int i = 0; i < CAL_SAMPLES; i++) {
    sx += analogRead(VRX_PIN); sy += analogRead(VRY_PIN); delay(10);
  }
  centerX = sx / CAL_SAMPLES; centerY = sy / CAL_SAMPLES;
  prefs.begin("joystick", false);
  prefs.putInt("centerX", centerX); prefs.putInt("centerY", centerY);
  prefs.end();
  computeThreshold();
  Serial.printf("Centro: VRx=%d VRy=%d  threshold=±%d — guardado\n", centerX, centerY, threshold);
}

void loadOrCalibrate() {
  prefs.begin("joystick", true);
  bool has = prefs.isKey("centerX");
  if (has) { centerX = prefs.getInt("centerX"); centerY = prefs.getInt("centerY"); }
  prefs.end();
  if (has) { computeThreshold(); Serial.printf("Cal cargada: VRx=%d VRy=%d threshold=±%d\n", centerX, centerY, threshold); }
  else runCalibration();
}

// ROT_90_CCW mount: dy = -(VRX - centerX).
// Physical up → VRX > centerX → dx positive → MAJOR.
ChordType joyChordType() {
  int dx = analogRead(VRX_PIN) - centerX;
  if (dx >  threshold) return MAJOR;
  if (dx < -threshold) return MINOR;
  return DOM7;
}

#define CHORD_DEBOUNCE_MS 80
ChordType debouncedChordType() {
  static ChordType stable = DOM7, pending = DOM7;
  static uint32_t pendingMs = 0;
  ChordType raw = joyChordType();
  if (raw != pending) { pending = raw; pendingMs = millis(); }
  else if (raw != stable && millis() - pendingMs >= CHORD_DEBOUNCE_MS) stable = raw;
  return stable;
}

// ─── display ─────────────────────────────────────────────────────────────────
char     shownLabel[12] = "";
int16_t  textX, textY;
uint16_t textW, textH;
uint32_t volShowUntil = 0;

void drawLabel(const char* label, uint8_t sz) {
  int16_t bx, by; uint16_t bw, bh;
  gfx->setTextSize(sz);
  gfx->getTextBounds(label, 0, 0, &bx, &by, &bw, &bh);
  int cx = (gfx->width()  - bw) / 2 - bx;
  int cy = (gfx->height() - bh) / 2 - by;
  gfx->setCursor(cx, cy);
  gfx->setTextColor(TEAL);
  gfx->print(label);
  textX = cx + bx; textY = cy + by; textW = bw; textH = bh;
}

void eraseLabel() { if (textW > 0) gfx->fillRect(textX, textY, textW, textH, BLACK); }

void updateDisplay(int rootSemi, ChordType ct) {
  if (millis() < volShowUntil) return;
  char label[12];
  if (rootSemi < 0) {
    strcpy(label, ".");
  } else {
    const char* N[]  = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
    const char* suf  = (ct == MAJOR) ? "maj" : (ct == MINOR) ? "min" : "7";
    snprintf(label, sizeof(label), "%s%s", N[rootSemi], suf);
  }
  if (!strcmp(label, shownLabel)) return;
  strcpy(shownLabel, label);
  eraseLabel();
  drawLabel(label, rootSemi < 0 ? 4 : 6);
}

// ─── globals ─────────────────────────────────────────────────────────────────
int       chordRoot = -1;
ChordType chordType = DOM7;

// ─── setup ───────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== Sketch 16: Omnichord ===");

  i2s.setPins(5 /*BCK*/, 7 /*LRCK*/, 6 /*DOUT*/, -1, -1);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!"); while (true) delay(1000);
  }

  pinMode(PIN_BL, OUTPUT); digitalWrite(PIN_BL, HIGH);
  if (!gfx->begin()) { Serial.println("Display failed!"); while (true) delay(1000); }
  gfx->fillScreen(BLACK);

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(PCF_ADDR);
  if (Wire.endTransmission() != 0) {
    gfx->setTextColor(RED); gfx->setTextSize(2);
    gfx->setCursor(8, gfx->height() / 2 - 16);
    gfx->printf("PCF8575 0x%02X", PCF_ADDR);
    gfx->setCursor(8, gfx->height() / 2 + 8);
    gfx->println("no encontrado");
    while (true) delay(1000);
  }
  stableState = prevRaw = readPCF();

  pinMode(JOY_SW, INPUT_PULLUP);
  analogReadResolution(12);
  if (digitalRead(JOY_SW) == LOW) runCalibration();
  else loadOrCalibrate();

  updateDisplay(-1, DOM7);
  Serial.println("Listo. PCF key = elige acorde (latch). Joystick L/R = strum. U/D = tipo.");
}

// ─── loop ────────────────────────────────────────────────────────────────────
void loop() {
  // ── joystick SW: cicla volumen ────────────────────────────────────────────
  static bool     prevSW = true;
  static uint32_t swMs   = 0;
  bool sw = digitalRead(JOY_SW);
  if (sw != prevSW) { swMs = millis(); prevSW = sw; }
  else if (!sw && swMs && millis() - swMs >= SW_DEBOUNCE_MS) {
    swMs = 0;
    volIdx    = (volIdx + 1) % 8;
    masterAmp = VOL_LEVELS[volIdx];
    char buf[8]; snprintf(buf, sizeof(buf), "VOL %d", volIdx + 1);
    eraseLabel(); drawLabel(buf, 5);
    volShowUntil = millis() + 800;
    shownLabel[0] = '\0';
    Serial.printf("Volumen: %d/8  (peak=%.0f)\n", volIdx + 1, masterAmp);
  }

  // ── chord type desde joystick ─────────────────────────────────────────────
  chordType = debouncedChordType();

  // ── PCF8575: detecta nueva tecla presionada (latching) ───────────────────
  uint16_t raw = readPCF();
  if (raw != prevRaw) { prevRaw = raw; lastChangeMs = millis(); }
  else if (raw != stableState && millis() - lastChangeMs >= DEBOUNCE_MS) {
    // Bits que pasaron de 1→0: tecla recién presionada (lógica activa-baja del PCF)
    uint16_t newPress = stableState & ~raw;
    stableState = raw;
    if (newPress) {
      for (int bit = 0; bit < 12; bit++) {
        if ((newPress >> bit) & 1) {
          const char* name = NOTE_NAME[bit];
          if (name) {
            int s = nameToSemi(name);
            if (s >= 0) {
              chordRoot = s;
              // Reafinar drone sin resetear fase → sin click en cambio de acorde
              drone.inc = 2.0f * PI * semiToFreq(chordRoot) / SAMPLE_RATE;
              drone.on  = true;
              Serial.printf("Acorde: %s%s\n",
                name,
                chordType == MAJOR ? "maj" : chordType == MINOR ? "min" : "7");
              break;
            }
          }
        }
      }
    }
  }

  // ── strum plate: eje Y del joystick (físico L/R en ROT_90_CCW) ───────────
  // Divide 0-4095 en N_STRUM zonas. Cruzar a una nueva zona dispara esa nota
  // con decay de arpa. Moverse en cualquier dirección produce notas.
  static int prevZone = -1;
  if (chordRoot >= 0) {
    int zone = constrain(analogRead(VRY_PIN) * N_STRUM / 4096, 0, N_STRUM - 1);
    if (prevZone >= 0 && zone != prevZone) {
      int semi = chordRoot + STRUM[chordType][zone];
      triggerVoice(zone, semi);
      Serial.printf("Strum zone %d — semi %d\n", zone, semi);
    }
    prevZone = zone;
  } else {
    prevZone = -1;
  }

  // ── display ───────────────────────────────────────────────────────────────
  updateDisplay(chordRoot, chordType);

  // ── audio ─────────────────────────────────────────────────────────────────
  audioBlock();
}
