// 10-audio-looper — step sequencer with live recording + step-entry editing.
//
// Builds on 09-audio-notes-gate: same I2S audio engine, same key map, same +/-key protocol.
//
// TWO MODES:
//   LIVE  — playing + recording: each note-on is OR'd into pattern[layer][curStep] immediately.
//   ENTRY — stopped: arrows / {/} navigate steps; note-on toggles a note in pattern[layer][entStep].
//
// PROTOCOL IN:
//   +key / -key    note on/off
//   \              play/stop
//   \r (Enter)     record toggle
//   [ / ]          prev/next layer (0–3)
//   { / }          prev/next entry step (stopped only)
//   N{hex}         jump to entry step 0-based hex (e.g. N3, Na)
//   V{0-9}         master volume
//   A{l}{0-9}      layer l volume (e.g. A07 = layer 0, level 7; A13 = layer 1, level 3)
//   B{nnn}\n       BPM  (e.g. B120\n)
//   T{nn}\n        steps per loop: 4/8/12/16/32 (e.g. T8\n, T16\n)
//   X              clear current layer's active step
//   Y{0-3}         clear all steps for layer n  (e.g. Y0)
//   Z              clear entire pattern
//   ?              dump full pattern + state (bridge sync on connect)
//
// PROTOCOL OUT:
//   S{n}                     playback advanced to step n
//   P{0|1}                   playing state
//   R{0|1}                   recording state
//   L{n}                     active layer (0–3)
//   E{n}                     entry step
//   T{n}                     active step count
//   A{l}{0-9}                layer l volume level (echo after set)
//   LA {v0} {v1} {v2} {v3}  all layer volumes (in ? dump)
//   PAT {l} {s0}…{sN}        32-char hex bitmask per step for layer l
//
// Pins: BCK=5, DOUT=6, LRCK=7 — same as 09-audio-notes-gate.

#include <ESP_I2S.h>
#include <math.h>
#include <ctype.h>

// ── pins ─────────────────────────────────────────────────────────────────
constexpr int PIN_BCLK = 5, PIN_DOUT = 6, PIN_LRCK = 7;

// ── audio ─────────────────────────────────────────────────────────────────
constexpr uint32_t SAMPLE_RATE = 44100;
constexpr int BLOCK = 128;
const float ATT = 1.0f / (SAMPLE_RATE * 0.005f);
const float REL = 1.0f / (SAMPLE_RATE * 0.040f);
const int VOL[10] = { 0, 20, 40, 70, 110, 160, 230, 320, 440, 600 };
float masterAmp = VOL[7];
constexpr uint32_t TAP_MS = 180;

// ── sequencer ─────────────────────────────────────────────────────────────
constexpr int NUM_LAYERS = 4;
constexpr int NUM_STEPS  = 32;

// ── key map ───────────────────────────────────────────────────────────────
// ⚠ Bit index == position in this array. Mirrored in app/looper-bridge.html.
struct KM { char c; int semi; };
const KM KEYMAP[] = {
  {'z', 0},{'s', 1},{'x', 2},{'d', 3},{'c', 4},{'v', 5},{'g', 6},{'b', 7},
  {'h', 8},{'n', 9},{'j',10},{'m',11},{',',12},{'l',13},{'.',14},{';',15},{'/',16},
  {'q',12},{'2',13},{'w',14},{'3',15},{'e',16},{'r',17},{'5',18},{'t',19},
  {'6',20},{'y',21},{'7',22},{'u',23},{'i',24},{'9',25},{'o',26},{'0',27},{'p',28},
};
constexpr int NUM_KEYS = sizeof(KEYMAP) / sizeof(KEYMAP[0]);

int keyIndex(char c) {
  for (int i = 0; i < NUM_KEYS; i++) if (KEYMAP[i].c == c) return i;
  return -1;
}
float semiToFreq(int s) { return 130.81f * powf(2.0f, s / 12.0f); }

// ── voice ─────────────────────────────────────────────────────────────────
struct Key { float phase=0, inc=0, env=0; bool on=false; uint32_t autoOff=0; };
Key keys[NUM_KEYS];
I2SClass i2s;

// ── state ─────────────────────────────────────────────────────────────────
uint64_t pattern[NUM_LAYERS][NUM_STEPS] = {};
uint64_t heldKeys  = 0;
int  curLayer  = 0;
int  curStep   = 0;
int  entStep   = 0;
int  numSteps  = 16;
bool playing   = false;
bool recording = false;
uint32_t bpm    = 120;
uint32_t stepMs = 60000UL / 120 / 4;
uint32_t lastStepTime = 0;

// per-layer volumes (0.0–1.0) and precomputed per-voice amplitude
float layerAmp[NUM_LAYERS] = { 1.0f, 1.0f, 1.0f, 1.0f };
float voiceAmp[NUM_KEYS];   // updated on step advance and volume change

// ── helpers ───────────────────────────────────────────────────────────────
uint64_t notesUnion(int step) {
  uint64_t m = 0;
  for (int l = 0; l < NUM_LAYERS; l++) m |= pattern[l][step];
  return m;
}

// Recompute per-voice amplitude from layer volumes.
// Voices in multiple layers sum their contributions (capped at 1.0).
// Voices not in any layer pattern (user-held improvisation) use masterAmp directly.
void recomputeVoiceAmps() {
  for (int j = 0; j < NUM_KEYS; j++) {
    float sum = 0.0f; bool any = false;
    for (int l = 0; l < NUM_LAYERS; l++) {
      if ((pattern[l][curStep] >> j) & 1) { sum += layerAmp[l]; any = true; }
    }
    voiceAmp[j] = masterAmp * (any ? fminf(sum, 1.0f) : 1.0f);
  }
}

void gateOn(int idx) {
  keys[idx].inc = TWO_PI * semiToFreq(KEYMAP[idx].semi) / SAMPLE_RATE;
  keys[idx].on  = true;
  keys[idx].autoOff = 0;
}

void gateOff(int idx, bool forced = false) {
  if (!forced && playing && ((notesUnion(curStep) >> idx) & 1)) return;
  keys[idx].on = false;
  keys[idx].autoOff = 0;
}

void applyNotes(uint64_t leaving, uint64_t arriving) {
  for (int j = 0; j < NUM_KEYS; j++) {
    bool was = (leaving >> j) & 1, is = (arriving >> j) & 1;
    if (!was && is) gateOn(j);
    // don't cut a note the user is physically holding — sustain freely across steps
    if (was && !is && !((heldKeys >> j) & 1)) gateOff(j, true);
  }
}

// ── sequencer control ─────────────────────────────────────────────────────
void advanceStep() {
  uint64_t leaving  = notesUnion(curStep);
  curStep = (curStep + 1) % numSteps;
  uint64_t arriving = notesUnion(curStep);
  applyNotes(leaving, arriving);
  recomputeVoiceAmps();
  Serial.print("S"); Serial.println(curStep);
}

void startPlay() {
  playing = true;
  curStep = numSteps - 1;
  lastStepTime = millis();
  advanceStep();
  Serial.println("P1");
}

void stopPlay() {
  playing = false;
  // Force all voices off directly — don't rely on pattern state, which may have been
  // cleared while notes were playing, leaving orphaned voices with no pattern entry.
  for (int j = 0; j < NUM_KEYS; j++) { keys[j].on = false; keys[j].autoOff = 0; }
  for (int j = 0; j < NUM_KEYS; j++) if ((heldKeys >> j) & 1) gateOn(j);
  recomputeVoiceAmps();
  Serial.println("P0");
}

// Gate off any voices that are sounding but no longer in the pattern or held by the user.
// Call after any pattern-clear operation during playback.
void gateOrphanedNotes() {
  if (!playing) return;
  uint64_t shouldSound = notesUnion(curStep) | heldKeys;
  for (int j = 0; j < NUM_KEYS; j++)
    if (keys[j].on && !((shouldSound >> j) & 1)) gateOff(j, true);
}

void setBpm(uint32_t b) {
  bpm    = constrain(b, 20, 300);
  stepMs = 60000UL / bpm / 4;
}

void setNumSteps(int n) {
  numSteps = constrain(n, 1, NUM_STEPS);
  if (curStep >= numSteps) curStep = 0;
  if (entStep >= numSteps) entStep = numSteps - 1;
  Serial.print("T"); Serial.println(numSteps);
}

void setLayerVol(int l, int level) {
  layerAmp[l] = (float)level / 9.0f;
  recomputeVoiceAmps();
  Serial.print("A"); Serial.print(l); Serial.println(level);
}

void dumpPattern() {
  Serial.print("T"); Serial.println(numSteps);
  // Layer volumes
  Serial.print("LA");
  for (int l = 0; l < NUM_LAYERS; l++) {
    int level = (int)roundf(layerAmp[l] * 9.0f);
    Serial.print(" "); Serial.print(level);
  }
  Serial.println();
  // Pattern
  for (int l = 0; l < NUM_LAYERS; l++) {
    Serial.print("PAT "); Serial.print(l);
    for (int s = 0; s < NUM_STEPS; s++) {
      uint64_t v = pattern[l][s];
      Serial.printf(" %08x%08x", (uint32_t)(v >> 32), (uint32_t)v);
    }
    Serial.println();
  }
}

// ── serial protocol ───────────────────────────────────────────────────────
void handleByte(int b) {
  static int  sign = 0;
  static enum { NORMAL, VOL_W, BPM_W, STEPS_W, STEP_W, LAYER_CLR_W,
                LAYER_VOL_L, LAYER_VOL_V } st = NORMAL;
  static char numBuf[8];
  static int  numLen = 0;
  static int  layerVolTarget = 0;

  char c = (char)b;

  if (st == VOL_W) {
    st = NORMAL;
    if (c >= '0' && c <= '9') { masterAmp = VOL[c - '0']; recomputeVoiceAmps(); }
    return;
  }
  if (st == BPM_W || st == STEPS_W) {
    if (c >= '0' && c <= '9' && numLen < 7) { numBuf[numLen++] = c; return; }
    numBuf[numLen] = '\0';
    if (numLen > 0) {
      if (st == BPM_W)   setBpm((uint32_t)atoi(numBuf));
      if (st == STEPS_W) setNumSteps(atoi(numBuf));
    }
    numLen = 0; st = NORMAL;
  }
  if (st == STEP_W) {
    st = NORMAL;
    int s = -1;
    if (c >= '0' && c <= '9') s = c - '0';
    else if (c >= 'a' && c <= 'f') s = c - 'a' + 10;
    else if (c >= 'A' && c <= 'F') s = c - 'A' + 10;
    if (s >= 0 && s < numSteps && !playing) {
      entStep = s; Serial.print("E"); Serial.println(entStep);
    }
    return;
  }
  if (st == LAYER_CLR_W) {
    st = NORMAL;
    int l = c - '0';
    if (l >= 0 && l < NUM_LAYERS)
      for (int s = 0; s < NUM_STEPS; s++) pattern[l][s] = 0;
    gateOrphanedNotes();
    return;
  }
  if (st == LAYER_VOL_L) {
    st = NORMAL;
    int l = c - '0';
    if (l >= 0 && l < NUM_LAYERS) { layerVolTarget = l; st = LAYER_VOL_V; }
    return;
  }
  if (st == LAYER_VOL_V) {
    st = NORMAL;
    if (c >= '0' && c <= '9') setLayerVol(layerVolTarget, c - '0');
    return;
  }

  if (c == '\n' || c == ' ') return;
  if (c == 'V') { st = VOL_W; return; }
  if (c == 'A') { st = LAYER_VOL_L; return; }
  if (c == 'B') { st = BPM_W; numLen = 0; return; }
  if (c == 'T') { st = STEPS_W; numLen = 0; return; }
  if (c == 'N') { st = STEP_W; return; }
  if (c == 'Y') { st = LAYER_CLR_W; return; }
  if (c == '+') { sign =  1; return; }
  if (c == '-') { sign = -1; return; }

  if (c == '\\') { if (playing) stopPlay(); else startPlay(); return; }
  if (c == '\r') {
    recording = !recording;
    Serial.print("R"); Serial.println(recording ? 1 : 0);
    return;
  }
  if (c == '[') {
    curLayer = (curLayer + NUM_LAYERS - 1) % NUM_LAYERS;
    Serial.print("L"); Serial.println(curLayer);
    return;
  }
  if (c == ']') {
    curLayer = (curLayer + 1) % NUM_LAYERS;
    Serial.print("L"); Serial.println(curLayer);
    return;
  }
  if (c == '{' && !playing) {
    entStep = (entStep + numSteps - 1) % numSteps;
    Serial.print("E"); Serial.println(entStep);
    return;
  }
  if (c == '}' && !playing) {
    entStep = (entStep + 1) % numSteps;
    Serial.print("E"); Serial.println(entStep);
    return;
  }
  if (c == 'X') {
    pattern[curLayer][playing ? curStep : entStep] = 0;
    gateOrphanedNotes();
    return;
  }
  if (c == 'Z') { memset(pattern, 0, sizeof(pattern)); gateOrphanedNotes(); return; }
  if (c == '?') { dumpPattern(); return; }

  int idx = keyIndex(tolower(c));
  if (idx < 0) { sign = 0; return; }

  if (sign == 1) {
    heldKeys |= (1ULL << idx);
    gateOn(idx);
    if (recording && playing)
      pattern[curLayer][curStep] |= (1ULL << idx);
    else if (!playing)
      pattern[curLayer][entStep] ^= (1ULL << idx);
  } else if (sign == -1) {
    heldKeys &= ~(1ULL << idx);
    gateOff(idx);
  } else {
    heldKeys |= (1ULL << idx);
    gateOn(idx);
    keys[idx].autoOff = millis() + TAP_MS;
    if (recording && playing)
      pattern[curLayer][curStep] |= (1ULL << idx);
    else if (!playing)
      pattern[curLayer][entStep] ^= (1ULL << idx);
  }
  sign = 0;
}

// ── audio block ───────────────────────────────────────────────────────────
void audioBlock() {
  int16_t buf[BLOCK * 2];
  for (int i = 0; i < BLOCK; i++) {
    int32_t acc = 0;
    for (int j = 0; j < NUM_KEYS; j++) {
      Key &k = keys[j];
      if (!k.on && k.env <= 0.0f) continue;
      float t = k.on ? 1.0f : 0.0f;
      if      (k.env < t) { k.env += ATT; if (k.env > t) k.env = t; }
      else if (k.env > t) { k.env -= REL; if (k.env < t) k.env = t; }
      if (k.env <= 0.0f) continue;
      acc += (int32_t)(voiceAmp[j] * k.env * sinf(k.phase));
      k.phase += k.inc;
      if (k.phase >= TWO_PI) k.phase -= TWO_PI;
    }
    int16_t s = (int16_t)constrain(acc, -32768, 32767);
    buf[i * 2] = s; buf[i * 2 + 1] = s;
  }
  i2s.write((uint8_t *)buf, sizeof(buf));
}

// ── setup / loop ──────────────────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(200);
  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed"); while (true) delay(1000);
  }
  recomputeVoiceAmps();
  Serial.println("Looper ready.");
  Serial.println("\\ play/stop  Enter rec  [/] layer  {/} step  N0-f step  V0-9 vol  A{l}{0-9} layer vol  B120 BPM  T8/16/32 steps  X clear step  Y0-3 clear layer  Z clear all  ? dump");
  dumpPattern();
}

void loop() {
  while (Serial.available()) handleByte(Serial.read());

  uint32_t now = millis();

  for (int j = 0; j < NUM_KEYS; j++) {
    if (keys[j].autoOff && now >= keys[j].autoOff) {
      keys[j].autoOff = 0;
      heldKeys &= ~(1ULL << j);
      gateOff(j);
    }
  }

  if (playing && now - lastStepTime >= stepMs) {
    advanceStep();
    lastStepTime += stepMs;
  }

  audioBlock();
}
