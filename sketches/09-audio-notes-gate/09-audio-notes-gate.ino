// 09-audio-notes-gate — polyphonic note-on / note-off over serial, with a REAL gate, plus
// momentary major/minor/min7 chord layers and a serial volume control.
//
// The problem this solves: a serial terminal sends *characters*, never key-up events — so
// holding a key (04-audio-notes, 07-chord-player) can only be faked with OS auto-repeat +
// a timeout. Here the host sends an explicit protocol so we get true note-on/off and true
// polyphony (hold keys → a sustained chord, each note releasing on its own key-up):
//
//   "+a" → note-on  for key 'a'      "-a" → note-off for key 'a'
//
// Chord layers are MOMENTARY (held, like Shift), not sticky:
//   ">" → major while held   "<" → minor while held   "{" → min7 while held   "}" → maj7 while held   "=" → back to single
// In a chord layer the synth is MONOPHONIC: the first key pressed owns one clean chord
// (root, 3rd, 5th [, 7th for min7]); extra keys are ignored so chords don't pile up. Releasing
// the layer ("=") collapses the held chord back to its single base note. The keyboard bridge maps
// arrow keys to these (ArrowUp = major, ArrowDown = minor, ArrowLeft = min7, ArrowRight = maj7, release = single).
//
// Volume: "V" followed by a digit '0'..'9' sets the level (see VOL[]); the bridge has a slider.
//
// ~2.5-octave layout across two QWERTY rows (tracker style, base C3):
//   lower — bottom-row whites + home-row blacks:  z=C3 s=C#3 x=D3 d=D#3 c=E3 v=F3 g=F#3 b=G3
//           h=G#3 n=A3 j=A#3 m=B3 ,=C4 l=C#4 .=D4 ;=D#4 /=E4
//   upper — top-letter-row whites + number-row blacks (one octave up): q=C4 2=C#4 w=D4 3=D#4
//           e=E4 r=F4 5=F#4 t=G4 6=G#4 y=A4 7=A#4 u=B4 i=C5 9=C#5 o=D5 0=D#5 p=E5
//
// Drive it with tools/keyboard-bridge.html. A bare key (no +/- prefix) still works as a tap.
//
// Pins (docs/hardware/wiring.md): I2S BCK=5, DIN=6, LRCK=7 (PCM5102 SCK->GND). Audio out goes through
// the PAM8403 to the speaker — keep the volume low (fixed-gain amp). See docs/hardware/modules/.

#include <ESP_I2S.h>
#include <math.h>
#include <ctype.h>

// Declared up here (before any function) so the Arduino IDE's auto-generated prototypes,
// which it inserts at the top of the file, can see the Layer and Key types.
enum Layer { SINGLE, MAJOR, MINOR, MIN7, MAJ7 };

// one key drives up to 4 oscillators (root, 3rd, 5th, 7th). Each oscillator has its OWN envelope
// so notes can be added/removed mid-sound (layer changes, note-off) with a click-free fade.
struct Key {
  float    phase[4] = {0, 0, 0, 0};
  float    inc[4]   = {0, 0, 0, 0};
  float    env[4]   = {0, 0, 0, 0};                      // per-oscillator level 0..1
  bool     on[4]    = {false, false, false, false};       // per-oscillator target (should it sound?)
  uint32_t autoOff  = 0;                                  // for bare-key taps; 0 = none
};

constexpr int PIN_BCLK = 5, PIN_DOUT = 6, PIN_LRCK = 7;
constexpr uint32_t SAMPLE_RATE = 44100;
constexpr int      BLOCK       = 128;     // frames per audio block (~3 ms)

// Volume levels (per-oscillator amplitude). 320 ≈ comfortable single note on the 4 Ohm /
// 3 W / 30 mm speaker (docs/hardware/modules/speaker.md). Index via "V<digit>" from the host.
const int VOL[10] = {0, 20, 40, 70, 110, 160, 230, 320, 440, 600};
float masterAmp = VOL[7];                 // default level 7 (320)

// Per-sample envelope ramps (attack ~5 ms, release ~40 ms) — avoids clicks on on/off.
const float ATT_STEP = 1.0f / (SAMPLE_RATE * 0.005f);
const float REL_STEP = 1.0f / (SAMPLE_RATE * 0.040f);

// NOTE: deliberately NO tone-shaping here (high-pass / limiter / polyphony scaling). Low chords
// sound "bloated" on the tiny 30 mm speaker because it can't reproduce deep bass — that's a
// hardware limit, kept out of this sketch on purpose. See docs/issues/01-audio-issues.md for
// the list of audio/output issues and where each fix belongs (a future DSP sketch, hardware, etc.).

constexpr uint32_t TAP_MS = 180;          // bare-key fallback: auto note-off after this

// ---------- key map ----------
// Each playable key → a semitone offset from C3 (semi 0 = C3 = 130.81 Hz). Two QWERTY rows;
// the upper row starts one octave above the lower (the C4..E4 octave overlaps on both).
// ⚠ This map is MIRRORED in tools/keyboard-bridge.html (LOWER/UPPER) — change both together.
struct KeyMap { char c; int semi; };
const KeyMap KEYMAP[] = {
  // lower: bottom row whites + home-row blacks
  {'z',0},{'s',1},{'x',2},{'d',3},{'c',4},{'v',5},{'g',6},{'b',7},{'h',8},{'n',9},{'j',10},{'m',11},
  {',',12},{'l',13},{'.',14},{';',15},{'/',16},
  // upper: top-letter-row whites + number-row blacks (one octave up)
  {'q',12},{'2',13},{'w',14},{'3',15},{'e',16},{'r',17},{'5',18},{'t',19},{'6',20},{'y',21},{'7',22},{'u',23},
  {'i',24},{'9',25},{'o',26},{'0',27},{'p',28},
};
constexpr int NUM_KEYS = sizeof(KEYMAP) / sizeof(KEYMAP[0]);

int keyIndex(char c) {
  for (int i = 0; i < NUM_KEYS; i++) if (KEYMAP[i].c == c) return i;
  return -1;
}
float semiToFreq(int semi) { return 130.81f * powf(2.0f, semi / 12.0f); }  // C3 = 130.81 Hz

// ---------- layer ----------
Layer layer = SINGLE;
int   chordOwner = -1;        // in a layer, the one key that owns the current triad (-1 = none)

// ---------- voices ----------
Key keys[NUM_KEYS];

I2SClass i2s;

void setOsc(Key &k, int sub, int semi) { k.inc[sub] = TWO_PI * semiToFreq(semi) / SAMPLE_RATE; }
bool held(const Key &k) { return k.on[0] || k.on[1] || k.on[2] || k.on[3]; }

// Note-on. In a chord layer, the first key becomes the monophonic chord owner (root,3rd,5th)
// and any further keys are ignored. In single mode, every key is an independent voice.
// Turning an oscillator "on" just fades its envelope up — no clicks, regardless of timing.
void gateOn(int idx) {
  Key &k = keys[idx];
  int root = KEYMAP[idx].semi;
  if (layer != SINGLE) {
    if (chordOwner >= 0 && chordOwner != idx && held(keys[chordOwner])) return;  // mono: ignore extra
    int third = (layer == MAJOR || layer == MAJ7) ? 4 : 3;
    setOsc(k, 0, root); setOsc(k, 1, root + third); setOsc(k, 2, root + 7);
    k.on[0] = k.on[1] = k.on[2] = true;
    if (layer == MIN7) { setOsc(k, 3, root + 10); k.on[3] = true; }
    else if (layer == MAJ7) { setOsc(k, 3, root + 11); k.on[3] = true; }
    else k.on[3] = false;
  } else {
    setOsc(k, 0, root);
    k.on[0] = true; k.on[1] = false; k.on[2] = false; k.on[3] = false;  // upper partials fade out
  }
  k.autoOff = 0;
  chordOwner = idx;   // remember the lead so a later layer change re-voices it live
}

void gateOff(int idx) {
  Key &k = keys[idx];
  k.on[0] = k.on[1] = k.on[2] = k.on[3] = false;   // fade all oscillators out
  k.autoOff = 0;
  if (idx == chordOwner) chordOwner = -1;
}

// Set (not toggle) the layer and RE-VOICE the held lead note live, so you can morph a note
// you're already holding between single / major / minor in any order. Adding/removing the
// 3rd & 5th fades them in/out (click-free); major<->minor just retunes the 3rd while it keeps
// sounding, so the transition is smooth.
void setLayer(Layer want) {
  if (want == layer) return;
  layer = want;
  if (chordOwner < 0 || !held(keys[chordOwner])) return;   // nothing held to re-voice
  Key &k = keys[chordOwner];
  int root = KEYMAP[chordOwner].semi;
  if (layer == SINGLE) {
    k.on[1] = k.on[2] = k.on[3] = false;  // fade out 3rd/5th/7th, keep the base note
  } else {
    int third = (layer == MAJOR || layer == MAJ7) ? 4 : 3;
    setOsc(k, 1, root + third);            // retune the 3rd (smooth if it's already sounding)
    setOsc(k, 2, root + 7);
    k.on[1] = k.on[2] = true;
    if (layer == MIN7)      { setOsc(k, 3, root + 10); k.on[3] = true; }
    else if (layer == MAJ7) { setOsc(k, 3, root + 11); k.on[3] = true; }
    else k.on[3] = false;                  // maj/min: fade out the 7th if it was sounding
  }
}

// Parse the protocol. 'V'+digit = volume. '>' '<' '=' = major/minor/single layer. '+' '-'
// set the sign for the next key char; a key char with no pending sign is a bare tap.
void handleByte(int b) {
  static int sign    = 0;   // +1 note-on, -1 note-off, 0 bare tap
  static bool volWait = false;
  char c = (char)b;

  if (volWait) { volWait = false; if (c >= '0' && c <= '9') masterAmp = VOL[c - '0']; return; }
  if (c == 'V') { volWait = true; return; }   // uppercase only — 'v' is the F note key
  if (c == '+') { sign = 1;  return; }
  if (c == '-') { sign = -1; return; }
  if (c == '>') { setLayer(MAJOR);  return; }
  if (c == '<') { setLayer(MINOR);  return; }
  if (c == '{') { setLayer(MIN7);   return; }
  if (c == '}') { setLayer(MAJ7);   return; }
  if (c == '=') { setLayer(SINGLE); return; }
  if (c == '\n' || c == '\r' || c == ' ') return;

  int idx = keyIndex(tolower(c));
  if (idx < 0) { sign = 0; return; }

  if      (sign == 1)  gateOn(idx);
  else if (sign == -1) gateOff(idx);
  else { gateOn(idx); keys[idx].autoOff = millis() + TAP_MS; }   // bare tap
  sign = 0;
}

// Generate one block. Always called (writes silence when idle) so the I2S DMA stays fed and
// serial stays responsive between blocks.
void audioBlock() {
  int16_t buf[BLOCK * 2];
  for (int i = 0; i < BLOCK; i++) {
    int32_t acc = 0;
    for (int j = 0; j < NUM_KEYS; j++) {
      Key &k = keys[j];
      if (!held(k) && k.env[0] <= 0.0f && k.env[1] <= 0.0f && k.env[2] <= 0.0f && k.env[3] <= 0.0f) continue;  // idle key
      for (int v = 0; v < 4; v++) {
        float target = k.on[v] ? 1.0f : 0.0f;
        if (k.env[v] < target)      { k.env[v] += ATT_STEP; if (k.env[v] > target) k.env[v] = target; }
        else if (k.env[v] > target) { k.env[v] -= REL_STEP; if (k.env[v] < target) k.env[v] = target; }
        if (k.env[v] <= 0.0f) continue;   // silent oscillator — skip

        acc += (int32_t)(masterAmp * k.env[v] * sinf(k.phase[v]));
        k.phase[v] += k.inc[v];
        if (k.phase[v] >= TWO_PI) k.phase[v] -= TWO_PI;
      }
    }
    int16_t s = (int16_t)constrain(acc, -32768, 32767);   // safety clamp
    buf[i * 2] = s; buf[i * 2 + 1] = s;
  }
  i2s.write((uint8_t *)buf, sizeof(buf));
}

void setup() {
  Serial.begin(115200);
  delay(200);

  i2s.setPins(PIN_BCLK, PIN_LRCK, PIN_DOUT, -1, -1);
  if (!i2s.begin(I2S_MODE_STD, SAMPLE_RATE, I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO)) {
    Serial.println("I2S init failed!");
    while (true) delay(1000);
  }
  Serial.println("\nNote gate v2 — +a/-a (on/off), hold '>' maj / '<' min / '{' min7 / '}' maj7 (mono chord), '=' single.");
  Serial.println("Volume: V0..V9.  ~2.5 octaves, base C3.  Use tools/keyboard-bridge.html.");
}

void loop() {
  while (Serial.available()) handleByte(Serial.read());

  // bare-key taps: release when their timer expires
  uint32_t now = millis();
  for (int j = 0; j < NUM_KEYS; j++)
    if (keys[j].autoOff && now >= keys[j].autoOff) gateOff(j);

  audioBlock();   // continuous audio (silence when idle)
}
