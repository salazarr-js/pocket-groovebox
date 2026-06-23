/*
 * Sketch 14 — PCF8575 keyboard → pantalla ST7789
 *
 * Muestra en la pantalla:
 *   ·       cuando ninguna tecla está presionada
 *   C       (nombre grande) mientras una tecla se mantiene
 *
 * Si se presionan varias teclas a la vez, muestra la de menor bit.
 *
 * SOLDADURA (orden invertido):
 *   P00=E  P01=F  P02=F# P03=G  P04=G# P05=A  P06=A# P07=B
 *   P10=D# P11=D  P12=C# P13=C  P14–P17=libres
 *
 * Wiring PCF8575 (docs/hardware/wiring.md):
 *   SDA=GPIO4  SCL=GPIO8  INT=GPIO15(no usado)  VCC=3V3  GND=GND
 *
 * Wiring ST7789 (docs/hardware/wiring.md):
 *   SCL=9  SDA=10  RES=11  DC=12  CS=13  BLK=14  VCC=3V3  GND=GND
 *
 * Librería requerida: "GFX Library for Arduino" (moononournation) — instalar
 * via Library Manager antes de compilar.
 */

#include <Wire.h>
#include <Arduino_GFX_Library.h>

// ─── colores ─────────────────────────────────────────────────────────────────
#define BLACK   RGB565_BLACK
#define WHITE   RGB565_WHITE
#define TEAL    0x05F7   // #00BFBD

// ─── display ─────────────────────────────────────────────────────────────────
#define PIN_SCLK 9
#define PIN_MOSI 10
#define PIN_RST  11
#define PIN_DC   12
#define PIN_CS   13
#define PIN_BL   14

Arduino_DataBus *bus = new Arduino_ESP32SPI(PIN_DC, PIN_CS, PIN_SCLK, PIN_MOSI, GFX_NOT_DEFINED);
Arduino_GFX    *gfx = new Arduino_ST7789(bus, PIN_RST, 1, true, 170, 320, 35, 0, 35, 0);
// rotation=1 → 90° CW → pantalla en horizontal (320×170)

// ─── PCF8575 ─────────────────────────────────────────────────────────────────
#define I2C_SDA   4
#define I2C_SCL   8
#define PCF_ADDR  0x20

// ─── config ───────────────────────────────────────────────────────────────────
#define DEBOUNCE_MS  8

// Caracteres de texto del display:
//   cada char en textSize N mide 6*N px ancho, 8*N px alto
#define NOTE_TEXT_SIZE   10   // nota grande: 60px ancho/char, 80px alto
#define IDLE_TEXT_SIZE    4   // punto idle: 24px ancho, 32px alto

// ─── mapeo bit → nota ────────────────────────────────────────────────────────
const char* NOTE_NAME[16] = {
  "E", "F", "F#", "G", "G#", "A", "A#", "B",   // P00–P07
  "D#", "D", "C#", "C",                          // P10–P13
  nullptr, nullptr, nullptr, nullptr,             // P14–P17 libres
};

// ─── estado ───────────────────────────────────────────────────────────────────
uint16_t prevRaw     = 0xFFFF;
uint16_t stableState = 0xFFFF;
uint32_t lastChangeMs = 0;

const char* displayedNote = "";   // "" = punto idle está en pantalla

// ─── helpers de display ──────────────────────────────────────────────────────

// Centra texto en la pantalla. charW = 6*textSize, charH = 8*textSize.
void drawCentered(const char* text, uint8_t textSize, uint16_t color) {
  int len   = strlen(text);
  int charW = 6 * textSize;
  int charH = 8 * textSize;
  int x = (gfx->width()  - len * charW) / 2;
  int y = (gfx->height() - charH)       / 2;
  gfx->setTextSize(textSize);
  gfx->setTextColor(color);
  gfx->setCursor(x, y);
  gfx->print(text);
}

void showNote(const char* note) {
  gfx->fillScreen(BLACK);
  if (note == nullptr || note[0] == '\0') {
    // idle — punto pequeño centrado
    drawCentered(".", IDLE_TEXT_SIZE, TEAL);
  } else {
    // nota grande centrada horizontalmente
    drawCentered(note, NOTE_TEXT_SIZE, TEAL);
  }
}

// ─── I2C ─────────────────────────────────────────────────────────────────────

uint16_t readPCF() {
  Wire.requestFrom((uint8_t)PCF_ADDR, (uint8_t)2);
  if (Wire.available() < 2) return 0xFFFF;
  uint8_t lo = Wire.read();
  uint8_t hi = Wire.read();
  return (uint16_t)lo | ((uint16_t)hi << 8);
}

// Devuelve el nombre de la primera tecla presionada (bit=0), o nullptr si ninguna.
const char* firstPressed(uint16_t state) {
  for (int bit = 0; bit < 12; bit++) {   // solo los 12 bits con tecla asignada
    if (!(state & (1u << bit))) return NOTE_NAME[bit];
  }
  return nullptr;
}

// ─── setup ───────────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  delay(500);

  // Display
  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, HIGH);
  gfx->begin();
  gfx->fillScreen(BLACK);

  // I2C
  Wire.begin(I2C_SDA, I2C_SCL);

  // Verificar que el chip responde
  Wire.beginTransmission(PCF_ADDR);
  bool ok = (Wire.endTransmission() == 0);
  if (!ok) {
    // Error — mostrar en pantalla y detener
    gfx->setTextColor(0xF800);   // rojo
    gfx->setTextSize(2);
    gfx->setCursor(8, gfx->height() / 2 - 24);
    gfx->println("PCF8575");
    gfx->setCursor(8, gfx->height() / 2);
    gfx->println("no encontrado");
    gfx->setTextSize(1);
    gfx->setCursor(8, gfx->height() / 2 + 28);
    gfx->printf("0x%02X en SDA=%d SCL=%d", PCF_ADDR, I2C_SDA, I2C_SCL);
    Serial.printf("PCF8575 no encontrado en 0x%02X\n", PCF_ADDR);
    while (true) delay(1000);
  }

  stableState = prevRaw = readPCF();
  showNote(nullptr);   // punto idle
  displayedNote = "";
}

// ─── loop ────────────────────────────────────────────────────────────────────

void loop() {
  uint16_t raw = readPCF();

  // debounce
  if (raw != prevRaw) {
    prevRaw = raw;
    lastChangeMs = millis();
    return;
  }
  if (raw == stableState) return;
  if (millis() - lastChangeMs < DEBOUNCE_MS) return;

  stableState = raw;

  const char* note = firstPressed(stableState);
  const char* shown = (displayedNote[0] == '\0') ? nullptr : displayedNote;

  if (note != shown) {
    showNote(note);
    displayedNote = (note == nullptr) ? "" : note;
  }

  delay(1);
}
