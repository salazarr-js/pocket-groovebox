/*
 * Sketch 13 — PCF8575 keyboard test (1 octava cromática)
 *
 * Lee un expansor PCF8575 cableado como teclado y imprime por consola
 * la nota que se presiona / suelta. Sin audio — solo validación de cableado.
 *
 * SOLDADURA REAL (orden invertido respecto a los docs):
 *   P00=E  P01=F  P02=F# P03=G  P04=G# P05=A  P06=A# P07=B
 *   P10=D# P11=D  P12=C# P13=C  P14–P17 = libres
 *
 * Bit=0 → tecla presionada (cierra a GND)
 * Bit=1 → tecla libre (pull-up interno del PCF8575)
 *
 * Wiring (docs/hardware/wiring.md):
 *   SDA → GPIO4  |  SCL → GPIO8  |  INT → GPIO15 (no usado en este sketch)
 *   VCC → 3V3    |  GND → GND
 *
 * Pines libres del expansor (P14–P17):
 *   Solo útiles para botones digitales de baja frecuencia.
 *   El joystick NO se puede mover acá (ejes analógicos necesitan ADC).
 *   Los encoders EC11 tampoco convienen (cuadratura por I2C pierde pasos).
 *   Ver el plan en ~/.claude/plans/ para el análisis completo.
 */

#include <Wire.h>

// ─── wiring ───────────────────────────────────────────────────────────────────
#define I2C_SDA   4
#define I2C_SCL   8
#define PCF_ADDR  0x20

// ─── config ───────────────────────────────────────────────────────────────────
#define OCTAVE        4     // número de octava para imprimir (ej. C4, D#4)
#define DEBOUNCE_MS   8     // ms mínimos para aceptar un cambio de estado

// ─── mapeo bit → nota ─────────────────────────────────────────────────────────
//
// Índice = número de bit (0–15). nullptr = pin libre (P14–P17).
// Ajustar este array si alguna tecla imprime la nota equivocada.
const char* NOTE_NAME[16] = {
  "E",   // bit 0  P00
  "F",   // bit 1  P01
  "F#",  // bit 2  P02
  "G",   // bit 3  P03
  "G#",  // bit 4  P04
  "A",   // bit 5  P05
  "A#",  // bit 6  P06
  "B",   // bit 7  P07
  "D#",  // bit 8  P10
  "D",   // bit 9  P11
  "C#",  // bit 10 P12
  "C",   // bit 11 P13
  nullptr, // bit 12 P14 — libre
  nullptr, // bit 13 P15 — libre
  nullptr, // bit 14 P16 — libre
  nullptr, // bit 15 P17 — libre
};

// ─── estado ───────────────────────────────────────────────────────────────────
uint16_t prevState   = 0xFFFF;  // todos en reposo (1 = libre)
uint16_t stableState = 0xFFFF;
uint32_t lastChangeMs = 0;

// ─── I2C ──────────────────────────────────────────────────────────────────────

uint16_t readPCF() {
  Wire.requestFrom((uint8_t)PCF_ADDR, (uint8_t)2);
  if (Wire.available() < 2) return 0xFFFF;   // error → asumir todo libre
  uint8_t lo = Wire.read();   // P00–P07
  uint8_t hi = Wire.read();   // P10–P17
  return (uint16_t)lo | ((uint16_t)hi << 8);
}

// ─── setup ────────────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  delay(500);

  Serial.println("=== PCF8575 keyboard test ===");

  Wire.begin(I2C_SDA, I2C_SCL);

  // Scan I2C — confirma que el chip responde y en qué dirección
  Serial.println("I2C scan:");
  bool found = false;
  for (uint8_t addr = 0x03; addr <= 0x77; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.printf("  0x%02X %s\n", addr, addr == PCF_ADDR ? "← PCF8575 (esperado)" : "");
      found = true;
    }
  }
  if (!found) Serial.println("  ningún dispositivo encontrado — revisar cableado SDA/SCL");

  stableState = prevState = readPCF();
  Serial.printf("Estado inicial: 0x%04X\n", stableState);
  Serial.println("Listo — presioná una tecla.\n");
}

// ─── loop ─────────────────────────────────────────────────────────────────────

void loop() {
  uint16_t raw = readPCF();

  // debounce: solo aceptar el estado si se mantiene estable DEBOUNCE_MS
  if (raw != prevState) {
    prevState = raw;
    lastChangeMs = millis();
    return;
  }
  if (raw == stableState) return;   // sin cambio respecto al estado confirmado
  if (millis() - lastChangeMs < DEBOUNCE_MS) return;

  // estado estabilizado — procesar cambios bit a bit
  uint16_t changed = stableState ^ raw;
  stableState = raw;

  for (int bit = 0; bit < 16; bit++) {
    if (!(changed & (1u << bit))) continue;

    if (NOTE_NAME[bit] == nullptr) {
      // pin libre — imprimir raw para diagnóstico
      bool pressed = !(raw & (1u << bit));
      Serial.printf("  [pin libre P%02d] %s  (raw=0x%04X)\n",
                    bit < 8 ? bit : bit - 8 + 10, pressed ? "↓" : "↑", raw);
      continue;
    }

    bool pressed = !(raw & (1u << bit));
    if (pressed) {
      Serial.printf("note-on:  %s%d\n", NOTE_NAME[bit], OCTAVE);
    } else {
      Serial.printf("note-off: %s%d\n", NOTE_NAME[bit], OCTAVE);
    }
  }

  delay(1);   // cede CPU brevemente
}
