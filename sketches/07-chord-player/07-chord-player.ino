// 07-chord-player — serial keyboard plays a note / major / minor on the PCM5102 and shows
// it on the ST7789 (landscape). Audio is continuous & non-blocking: a note sustains while
// the key is (auto-)repeated and fades shortly after the last keypress. Another experiment.
//
// Root keys (one octave, C..B with sharps):
//   z=C  s=C#  x=D  d=D#  c=E  v=F  g=F#  b=G  h=G#  n=A  j=A#  m=B
//
// Layers (sticky, like Caps) — set with the arrows; press the same arrow again to go back:
//   base = single note   ->  "C"
//   Up   = major triad    ->  "Cmaj"   (root, +4, +7 semitones)
//   Down = minor triad    ->  "Cmin"   (root, +3, +7 semitones)
//   (Arduino Serial Monitor can't send arrows — use a raw terminal like `screen`, or the
//    fallback keys '.' = major, ',' = minor.)
//
// HOLD vs RELEASE: a serial terminal has no "key up" event, and holding a key relies on the
// OS auto-repeat (with an initial delay). So "sound while held / stop on release" is only
// approximated here — true behavior comes later from the physical GPIO keyboard. Tune
// RELEASE_MS and your OS key-repeat speed for the best feel.
//
// Pins (docs/hardware/wiring.md): DAC I2S BCK=5 DIN=6 LCK=7 (SCK->GND). Display SPI SCL=9 SDA=10
// RES=11 DC=12 CS=13 BLK=14. VCC/VIN=3V3, GND=GND.

#include <ESP_I2S.h>
#include <Arduino_GFX_Library.h>
#include <math.h>
#include <ctype.h>

// Arduino_GFX prefixes its colors with RGB565_; alias the few we use + a container tint.
#define BLACK         RGB565_BLACK
#define WHITE         RGB565_WHITE
#define ORANGE        RGB565_ORANGE
#define COL_CONTAINER RGB565(40, 44, 58)

// ---------- display ----------
constexpr int PIN_SCLK = 9, PIN_MOSI = 10, PIN_RST = 11, PIN_DC = 12, PIN_CS = 13, PIN_BL = 14;
Arduino_DataBus *bus = new Arduino_ESP32SPI(PIN_DC, PIN_CS, PIN_SCLK, PIN_MOSI, GFX_NOT_DEFINED);
// rotation 1 = landscape (320x170). If shifted, tweak offsets (35,0,35,0); if upside down, use 3.
Arduino_GFX *gfx = new Arduino_ST7789(bus, PIN_RST, 1, true /*IPS*/, 170, 320, 35, 0, 35, 0);

// ---------- audio ----------
constexpr int PIN_BCLK = 5, PIN_DOUT = 6, PIN_LRCK = 7;
constexpr uint32_t SAMPLE_RATE = 44100;
constexpr int      BLOCK       = 128;     // frames generated per loop pass (~3 ms)
constexpr float    PEAK        = 8000.0f; // total amplitude (split across voices)
constexpr uint32_t RELEASE_MS  = 150;     // note-off this long after the last keypress
I2SClass i2s;

// per-sample envelope ramp steps (attack ~4 ms, release ~12 ms) — avoids clicks
const float ATT_STEP = 1.0f / (SAMPLE_RATE * 0.004f);
const float REL_STEP = 1.0f / (SAMPLE_RATE * 0.012f);

const char *NOTE_NAMES[12] = {"C","C#","D","D#","E","F","F#","G","G#","A","A#","B"};
enum Layer { SINGLE, MAJOR, MINOR };

// ---------- state ----------
float    phs[3]   = {0, 0, 0};
float    incs[3]  = {0, 0, 0};
int      numVoices = 0;
float    voiceAmp = 0;
float    env      = 0;        // current amplitude 0..1
bool     noteOn   = false;    // envelope target (true = ramp up)
uint32_t lastKeyMs = 0;
int      rootSemi = -1;
Layer    layer    = SINGLE;

float semiToFreq(int semi) { return 261.63f * powf(2.0f, semi / 12.0f); }  // C4 = 261.63 Hz

int keyToSemi(char c) {
  switch (c) {
    case 'z': return 0;   case 's': return 1;   case 'x': return 2;   case 'd': return 3;
    case 'c': return 4;   case 'v': return 5;   case 'g': return 6;   case 'b': return 7;
    case 'h': return 8;   case 'n': return 9;   case 'j': return 10;  case 'm': return 11;
    default:  return -1;
  }
}

// (Re)build the voices for the current root + layer and (re)start the note. Phases are left
// running so re-triggers and pitch changes don't click.
void triggerNote() {
  if (rootSemi < 0) return;
  int semis[3];
  if (layer == SINGLE) { semis[0] = rootSemi; numVoices = 1; }
  else {
    int third = (layer == MAJOR) ? 4 : 3;
    semis[0] = rootSemi; semis[1] = rootSemi + third; semis[2] = rootSemi + 7;
    numVoices = 3;
  }
  for (int v = 0; v < numVoices; v++) incs[v] = 2.0f * PI * semiToFreq(semis[v]) / SAMPLE_RATE;
  voiceAmp = PEAK / numVoices;
  noteOn = true;
  lastKeyMs = millis();
}

// Generate one block of audio. Always called — writes silence (env→0) when idle, which keeps
// the I2S DMA fed and paced so serial stays responsive between blocks.
void audioBlock() {
  int16_t buf[BLOCK * 2];
  for (int i = 0; i < BLOCK; i++) {
    float target = noteOn ? 1.0f : 0.0f;
    if (env < target)      { env += ATT_STEP; if (env > target) env = target; }
    else if (env > target) { env -= REL_STEP; if (env < target) env = target; }

    float mix = 0;
    for (int v = 0; v < numVoices; v++) {
      mix += sinf(phs[v]);
      phs[v] += incs[v];
      if (phs[v] >= 2.0f * PI) phs[v] -= 2.0f * PI;
    }
    int16_t s = (int16_t)(voiceAmp * env * mix);
    buf[i * 2] = s; buf[i * 2 + 1] = s;
  }
  i2s.write((uint8_t *)buf, sizeof(buf));
}

// ---------- UI (landscape 320x170) ----------
constexpr int CX = 6, CY = 6, CW = 308, CH = 158, CR = 14;
constexpr int PAD = 16;
constexpr int DIV_Y = CY + 44;

void drawScaffold() {
  gfx->fillScreen(BLACK);
  gfx->fillRoundRect(CX, CY, CW, CH, CR, COL_CONTAINER);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(CX + PAD, CY + 12);     gfx->print("Pocket Groovebox");
  gfx->setCursor(CX + PAD + 1, CY + 12); gfx->print("Pocket Groovebox");   // faux-bold
  gfx->fillRect(CX + PAD, DIV_Y, CW - 2 * PAD, 2, ORANGE);                  // 2px divider
}

// Redraw the centered chord label, but only when it actually changes (so holding a key
// doesn't repaint over and over and starve the audio).
void updateDisplay() {
  char label[8];
  if (rootSemi < 0) strcpy(label, "--");
  else {
    const char *suf = (layer == MAJOR) ? "maj" : (layer == MINOR) ? "min" : "";
    snprintf(label, sizeof(label), "%s%s", NOTE_NAMES[rootSemi], suf);
  }

  static char shown[8] = "";
  if (strcmp(label, shown) == 0) return;   // nothing changed
  strcpy(shown, label);

  const int ax = CX + CR, aw = CW - 2 * CR;
  const int ay = DIV_Y + 4, ah = (CY + CH - CR) - ay;
  gfx->fillRect(ax, ay, aw, ah, COL_CONTAINER);

  gfx->setTextSize(4);
  gfx->setTextColor(WHITE);
  int16_t bx, by; uint16_t bw, bh;
  gfx->getTextBounds(label, 0, 0, &bx, &by, &bw, &bh);
  gfx->setCursor(CX + (CW - bw) / 2 - bx, ay + (ah - bh) / 2 - by);
  gfx->print(label);
}

// toggle a layer on/off (pressing the active arrow again returns to SINGLE)
void setLayer(Layer want) {
  layer = (layer == want) ? SINGLE : want;
  updateDisplay();
  if (rootSemi >= 0) triggerNote();   // re-sound the held note in the new layer
}

void handleByte(int b) {
  // arrow escape sequence: ESC '[' 'A'(up) / 'B'(down)
  static int esc = 0;
  if (esc == 0 && b == 0x1B) { esc = 1; return; }
  if (esc == 1) { esc = (b == '[') ? 2 : 0; return; }
  if (esc == 2) { esc = 0; if (b == 'A') setLayer(MAJOR); else if (b == 'B') setLayer(MINOR); return; }

  char c = tolower((char)b);
  if (c == '.') { setLayer(MAJOR); return; }
  if (c == ',') { setLayer(MINOR); return; }
  if (c == '\n' || c == '\r') return;

  int semi = keyToSemi(c);
  if (semi < 0) return;
  rootSemi = semi;       // (auto-repeat of the same key just keeps re-triggering → sustain)
  updateDisplay();
  triggerNote();
}

void setup() {
  Serial.begin(115200);
  delay(200);

  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!");
    while (true) delay(1000);
  }

  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, HIGH);
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
    while (true) delay(1000);
  }
  drawScaffold();
  updateDisplay();

  Serial.println("\nChord player — hold a root key (z s x d c v g b h n j m), arrows = maj/min layer");
}

void loop() {
  while (Serial.available()) handleByte(Serial.read());

  // approximate key-release: drop the note if no key arrived recently
  if (noteOn && millis() - lastKeyMs > RELEASE_MS) noteOn = false;

  audioBlock();   // continuous audio (silence when idle)
}
