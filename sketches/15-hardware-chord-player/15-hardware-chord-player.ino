/*
 * Sketch 15 — Hardware chord player
 *
 * Primera integración completa sin laptop:
 *   - Teclado PCF8575 (una octava soldada) → root note
 *   - Joystick KY-023: up=mayor, down=menor, centro=nota sola
 *   - Joystick SW: cicla volumen 1→2→…→8→1 (escala logarítmica ~3 dB/paso)
 *   - Pantalla ST7789: · / C / Cmaj / Cmin; overlay "VOL 5" al cambiar volumen
 *   - Audio I2S PCM5102
 *
 * Joystick SW al boot → fuerza recalibración del centro (guardado en NVS).
 *
 * SOLDADURA PCF8575 (orden invertido respecto a docs):
 *   P00=E  P01=F  P02=F# P03=G  P04=G# P05=A  P06=A# P07=B
 *   P10=D# P11=D  P12=C# P13=C  P14–P17=libres
 *
 * Wiring (docs/hardware/wiring.md):
 *   I2S    BCK=5   DIN=6   LRCK=7
 *   SPI    SCL=9   SDA=10  RES=11  DC=12  CS=13  BLK=14
 *   I2C    SDA=4   SCL=8
 *   Joy    VRx=2   VRy=1   SW=21
 *
 * Librerías: "GFX Library for Arduino" (moononournation), ESP_I2S (incluida con core ESP32).
 */

#include <Wire.h>
#include <Arduino_GFX_Library.h>
#include <ESP_I2S.h>
#include <Preferences.h>
#include <math.h>

// ─── tipos globales (deben estar antes de los prototipos auto del IDE) ────────
enum Layer { SINGLE,
             MAJOR,
             MINOR };

// ─── colores ─────────────────────────────────────────────────────────────────
#define BLACK RGB565_BLACK
#define WHITE RGB565_WHITE
#define RED RGB565_RED
#define TEAL 0x05F7  // #00BFBD

// ─── display ─────────────────────────────────────────────────────────────────
#define PIN_SCLK 9
#define PIN_MOSI 10
#define PIN_RST 11
#define PIN_DC 12
#define PIN_CS 13
#define PIN_BL 14

Arduino_DataBus* bus = new Arduino_ESP32SPI(PIN_DC, PIN_CS, PIN_SCLK, PIN_MOSI, GFX_NOT_DEFINED);
Arduino_GFX* gfx = new Arduino_ST7789(bus, PIN_RST, 1, true, 170, 320, 35, 0, 35, 0);
// rotation=1 → 90° CW → landscape 320×170

// ─── audio ───────────────────────────────────────────────────────────────────
#define PIN_BCLK 5
#define PIN_DOUT 6
#define PIN_LRCK 7

constexpr uint32_t SAMPLE_RATE = 44100;
constexpr int BLOCK = 128;

// Envolventes:
//   ATT  4 ms  — ataque al inicio de nota
//   REL  20 ms — release natural al soltar tecla
//   XFADE 2 ms — micro-fadeout antes de cambiar parámetros de audio;
//                2 ms es imperceptible como silencio pero evita todo click
const float ATT_STEP = 1.0f / (SAMPLE_RATE * 0.004f);
const float REL_STEP = 1.0f / (SAMPLE_RATE * 0.020f);
const float XFADE_STEP = 1.0f / (SAMPLE_RATE * 0.002f);

I2SClass i2s;

float phs[3] = { 0, 0, 0 };
float incs[3] = { 0, 0, 0 };
int numVoices = 1;
float voiceAmp = 0;
float env = 0;
bool noteOn = false;

// Crossfade: cuando hay que cambiar frecuencias/voces sin click,
// se hace un micro-fadeout a 0 y se dispara el retrigger al llegar.
bool retriggering = false;
int pendingSemi = -1;
Layer pendingLayer = SINGLE;

float semiToFreq(int semi) {
  return 261.63f * powf(2.0f, semi / 12.0f);
}

// ─── volumen (escala logarítmica ~3 dB por paso, factor ≈ √2) ────────────────
//
// Techo medido en el speaker interno: ~800 (>800 distorsiona).
const float VOL_LEVELS[8] = { 70, 100, 140, 200, 285, 400, 565, 800 };
int volIdx = 7;
float peakVolume = VOL_LEVELS[7];

// ─── PCF8575 ─────────────────────────────────────────────────────────────────
#define I2C_SDA 4
#define I2C_SCL 8
#define PCF_ADDR 0x20
#define DEBOUNCE_MS 8

const char* NOTE_NAME[16] = {
  "E",
  "F",
  "F#",
  "G",
  "G#",
  "A",
  "A#",
  "B",
  "D#",
  "D",
  "C#",
  "C",
  nullptr,
  nullptr,
  nullptr,
  nullptr,
};

int nameToSemi(const char* name) {
  const char* N[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
  for (int i = 0; i < 12; i++)
    if (strcmp(name, N[i]) == 0) return i;
  return -1;
}

uint16_t prevRaw = 0xFFFF;
uint16_t stableState = 0xFFFF;
uint32_t lastChangeMs = 0;

uint16_t readPCF() {
  Wire.requestFrom((uint8_t)PCF_ADDR, (uint8_t)2);
  if (Wire.available() < 2) return 0xFFFF;
  uint8_t lo = Wire.read();
  uint8_t hi = Wire.read();
  return (uint16_t)lo | ((uint16_t)hi << 8);
}

const char* firstPressed(uint16_t state) {
  for (int bit = 0; bit < 12; bit++)
    if (!(state & (1u << bit))) return NOTE_NAME[bit];
  return nullptr;
}

// ─── joystick ────────────────────────────────────────────────────────────────
#define VRX_PIN 2
#define VRY_PIN 1
#define JOY_SW 21
#define THRESHOLD_PCT 25
#define CAL_SAMPLES 50
#define SW_DEBOUNCE_MS 50

enum Mount { ROT_0,
             ROT_90_CW,
             ROT_180,
             ROT_90_CCW };
#define JOY_MOUNT ROT_90_CCW  // verificado en placa de test (sketch 12)

Preferences prefs;
int centerX, centerY, threshold;

void applyMount(int rvx, int rvy, float& dx, float& dy) {
  switch (JOY_MOUNT) {
    case ROT_0:
      dx = -rvx;
      dy = -rvy;
      break;
    case ROT_90_CW:
      dx = -rvy;
      dy = rvx;
      break;
    case ROT_180:
      dx = rvx;
      dy = rvy;
      break;
    case ROT_90_CCW:
      dx = rvy;
      dy = -rvx;
      break;
  }
}

void computeThreshold() {
  int minRange = min({ centerX, centerY, 4095 - centerX, 4095 - centerY });
  threshold = (minRange * THRESHOLD_PCT) / 100;
}

void runCalibration() {
  Serial.println("Calibrando joystick — no tocar...");
  delay(1000);
  long sumX = 0, sumY = 0;
  for (int i = 0; i < CAL_SAMPLES; i++) {
    sumX += analogRead(VRX_PIN);
    sumY += analogRead(VRY_PIN);
    delay(10);
  }
  centerX = sumX / CAL_SAMPLES;
  centerY = sumY / CAL_SAMPLES;
  prefs.begin("joystick", false);
  prefs.putInt("centerX", centerX);
  prefs.putInt("centerY", centerY);
  prefs.end();
  computeThreshold();
  Serial.printf("Centro: VRx=%d VRy=%d  threshold=±%d — guardado\n", centerX, centerY, threshold);
}

void loadOrCalibrate() {
  prefs.begin("joystick", true);
  bool hasCal = prefs.isKey("centerX");
  if (hasCal) {
    centerX = prefs.getInt("centerX");
    centerY = prefs.getInt("centerY");
  }
  prefs.end();
  if (hasCal) {
    computeThreshold();
    Serial.printf("Calibración cargada: VRx=%d VRy=%d  threshold=±%d\n", centerX, centerY, threshold);
  } else {
    runCalibration();
  }
}

// Si UP/DOWN salen invertidos en hardware: swapear los signos de dy.
Layer joyLayer() {
  float dx, dy;
  applyMount(analogRead(VRX_PIN) - centerX, analogRead(VRY_PIN) - centerY, dx, dy);
  if (dy < -threshold) return MAJOR;
  if (dy > threshold) return MINOR;
  return SINGLE;
}

// Al mover el joy de UP a DOWN pasa por CENTER ~20-40 ms. Sin debounce eso
// dispara SINGLE brevemente y se oye como ruido entre acordes.
#define LAYER_DEBOUNCE_MS 80

Layer debouncedJoyLayer() {
  static Layer stable = SINGLE;
  static Layer pending = SINGLE;
  static uint32_t pendingMs = 0;

  Layer raw = joyLayer();
  if (raw != pending) {
    pending = raw;
    pendingMs = millis();
  } else if (raw != stable && millis() - pendingMs >= LAYER_DEBOUNCE_MS) {
    stable = raw;
  }
  return stable;
}

// ─── audio ───────────────────────────────────────────────────────────────────

// Aplica inmediatamente las frecuencias y voces. Solo llamar cuando env == 0
// (o al inicio) para evitar clicks.
void applyTrigger(int semi, Layer layer) {
  int semis[3];
  if (layer == SINGLE) {
    semis[0] = semi;
    numVoices = 1;
  } else {
    int third = (layer == MAJOR) ? 4 : 3;
    semis[0] = semi;
    semis[1] = semi + third;
    semis[2] = semi + 7;
    numVoices = 3;
  }
  for (int v = 0; v < numVoices; v++) {
    incs[v] = 2.0f * PI * semiToFreq(semis[v]) / SAMPLE_RATE;
    phs[v] = 0.0f;  // resetear fase: sin(0)=0, sin discontinuity en amplitud
  }
  voiceAmp = peakVolume;
  noteOn = true;
}

// Programa un cambio de nota/acorde con micro-crossfade de 2 ms.
// Si ya estamos en silencio, aplica directo. Si hay sonido, hace
// fadeout a 0 y dispara el retrigger al llegar — sin click en ningún caso.
void scheduleTrigger(int semi, Layer layer) {
  if (env < 0.01f) {
    applyTrigger(semi, layer);
  } else {
    pendingSemi = semi;
    pendingLayer = layer;
    retriggering = true;
    noteOn = false;  // inicia micro-fadeout
  }
}

void audioBlock() {
  int16_t buf[BLOCK * 2];
  for (int i = 0; i < BLOCK; i++) {
    float target = noteOn ? 1.0f : 0.0f;

    // Usar XFADE_STEP durante el micro-fadeout; REL_STEP para release normal.
    float downStep = retriggering ? XFADE_STEP : REL_STEP;

    if (env < target) {
      env += ATT_STEP;
      if (env > target) env = target;
    } else if (env > target) {
      env -= downStep;
      if (env < target) env = target;
    }

    // Cuando el micro-fadeout llega a silencio, disparar el retrigger pendiente.
    if (retriggering && env <= 0.0f) {
      applyTrigger(pendingSemi, pendingLayer);
      retriggering = false;
    }

    // Mix normalizado por numVoices → amplitud constante sin importar cuántas voces.
    float mix = 0;
    for (int v = 0; v < numVoices; v++) {
      mix += sinf(phs[v]);
      phs[v] += incs[v];
      if (phs[v] >= 2.0f * PI) phs[v] -= 2.0f * PI;
    }
    int16_t s = (int16_t)(voiceAmp * env * (mix / numVoices));
    buf[i * 2] = s;
    buf[i * 2 + 1] = s;
  }
  i2s.write((uint8_t*)buf, sizeof(buf));
}

// ─── display ─────────────────────────────────────────────────────────────────
//
// Se evita fillScreen() durante el loop: solo se borra el área del texto anterior
// (fillRect sobre los bounds del label previo). fillScreen completo en 320×170
// tarda ~15 ms y deja el DMA de I2S sin datos → glitch de audio.

char shownLabel[8] = "";
int16_t textX = 0, textY = 0;
uint16_t textW = 0, textH = 0;
uint32_t volShowUntil = 0;

void drawCentered(const char* label, uint8_t sz) {
  int16_t bx, by;
  uint16_t bw, bh;
  gfx->setTextSize(sz);
  gfx->getTextBounds(label, 0, 0, &bx, &by, &bw, &bh);
  int cx = (gfx->width() - bw) / 2 - bx;
  int cy = (gfx->height() - bh) / 2 - by;
  gfx->setCursor(cx, cy);
  gfx->setTextColor(TEAL);
  gfx->print(label);
  textX = cx + bx;
  textY = cy + by;
  textW = bw;
  textH = bh;
}

void eraseText() {
  if (textW > 0) gfx->fillRect(textX, textY, textW, textH, BLACK);
}

void showVolumeOverlay() {
  char buf[8];
  snprintf(buf, sizeof(buf), "VOL %d", volIdx + 1);
  eraseText();
  drawCentered(buf, 5);
  volShowUntil = millis() + 800;
  shownLabel[0] = '\0';
  Serial.printf("Volumen: %d/8  (peak=%.0f)\n", volIdx + 1, peakVolume);
}

void updateDisplay(int rootSemi, Layer layer) {
  if (millis() < volShowUntil) return;

  char label[8];
  if (rootSemi < 0) {
    strcpy(label, ".");
  } else {
    const char* N[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
    const char* suf = (layer == MAJOR) ? "maj" : (layer == MINOR) ? "min"
                                                                  : "";
    snprintf(label, sizeof(label), "%s%s", N[rootSemi], suf);
  }

  if (strcmp(label, shownLabel) == 0) return;
  strcpy(shownLabel, label);

  eraseText();
  drawCentered(label, (rootSemi < 0) ? 4 : 6);
}

// ─── setup ───────────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println("=== Hardware chord player ===");

  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!");
    while (true) delay(1000);
  }

  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, HIGH);
  if (!gfx->begin()) {
    Serial.println("Display init failed!");
    while (true) delay(1000);
  }
  gfx->fillScreen(BLACK);  // fillScreen solo en setup, cuando no corre audio

  Wire.begin(I2C_SDA, I2C_SCL);
  Wire.beginTransmission(PCF_ADDR);
  if (Wire.endTransmission() != 0) {
    gfx->setTextColor(RED);
    gfx->setTextSize(2);
    gfx->setCursor(8, gfx->height() / 2 - 16);
    gfx->printf("PCF8575 0x%02X", PCF_ADDR);
    gfx->setCursor(8, gfx->height() / 2 + 8);
    gfx->println("no encontrado");
    Serial.printf("PCF8575 no encontrado en 0x%02X\n", PCF_ADDR);
    while (true) delay(1000);
  }
  stableState = prevRaw = readPCF();

  pinMode(JOY_SW, INPUT_PULLUP);
  analogReadResolution(12);
  if (digitalRead(JOY_SW) == LOW) {
    Serial.println("SW presionado — forzando recalibración...");
    runCalibration();
  } else {
    loadOrCalibrate();
  }

  Serial.printf("Volumen inicial: %d/8\n", volIdx + 1);
  updateDisplay(-1, SINGLE);
  Serial.println("Listo.");
}

// ─── loop ────────────────────────────────────────────────────────────────────

void loop() {
  // ── joystick SW: cicla volumen ──────────────────────────────────────────────
  static bool prevSW = true;
  static uint32_t swChangeMs = 0;
  bool sw = digitalRead(JOY_SW);
  if (sw != prevSW) {
    swChangeMs = millis();
    prevSW = sw;
  } else if (!sw && swChangeMs != 0 && millis() - swChangeMs >= SW_DEBOUNCE_MS) {
    swChangeMs = 0;
    volIdx = (volIdx + 1) % 8;
    peakVolume = VOL_LEVELS[volIdx];
    showVolumeOverlay();
    if (noteOn) voiceAmp = peakVolume;
  }

  // ── layer por joystick ──────────────────────────────────────────────────────
  Layer layer = debouncedJoyLayer();

  // ── PCF8575 debounceado ─────────────────────────────────────────────────────
  uint16_t raw = readPCF();
  if (raw != prevRaw) {
    prevRaw = raw;
    lastChangeMs = millis();
  } else if (raw != stableState && millis() - lastChangeMs >= DEBOUNCE_MS) {
    stableState = raw;
  }

  const char* noteName = firstPressed(stableState);
  int rootSemi = noteName ? nameToSemi(noteName) : -1;

  // ── actualizar display y audio si cambió algo ───────────────────────────────
  static int prevSemi = -2;
  static Layer prevLayer = SINGLE;

  if (rootSemi != prevSemi || layer != prevLayer) {
    prevSemi = rootSemi;
    prevLayer = layer;
    updateDisplay(rootSemi, layer);
    if (rootSemi >= 0) scheduleTrigger(rootSemi, layer);
    else {
      noteOn = false;
      retriggering = false;
    }
  } else {
    updateDisplay(rootSemi, layer);  // re-evalúa cuando expira overlay de volumen
  }

  audioBlock();
}
