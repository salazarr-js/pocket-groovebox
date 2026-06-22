/*
 * Sketch 12 — KY-023 joystick test
 *
 * Calibration is saved to flash (NVS via Preferences).
 * On boot: if saved data exists it is used directly.
 * To force recalibration: hold SW pressed while powering on.
 *
 * Wiring (FIXED — does not change with orientation):
 *   VRx → GPIO2  |  VRy → GPIO1  |  SW → GPIO21
 *   VCC → 3V3 ⚠️ NOT 5V  |  GND → GND
 *
 * Mount orientation: a single knob, JOY_MOUNT.
 *   Read the fixed pins, subtract the calibrated centers (signed deltas),
 *   and applyMount() rotates those deltas into the "world" frame (+x=right, +y=down).
 *   A single step resolves both axis SWAP and POLARITY — they can never drift
 *   out of sync like they did with separate pin-swap + invert flags.
 *   The test board is mounted 90° CCW → JOY_MOUNT = ROT_90_CCW (verified).
 *   See ky023.md for the orientation table.
 */

#include <math.h>
#include <Preferences.h>

// ─── fixed wiring ─────────────────────────────────────────────────────────────
#define VRX_PIN       2   // VRx — always GPIO2, independent of orientation
#define VRY_PIN       1   // VRy — always GPIO1
#define JOY_SW_PIN    21
#define THRESHOLD_PCT 25
#define CAL_SAMPLES   50

// ─── mount orientation (the only knob that changes when remounting) ───────────
enum Mount { ROT_0, ROT_90_CW, ROT_180, ROT_90_CCW };
#define JOY_MOUNT  ROT_90_CCW   // test board: labels facing down

// ─────────────────────────────────────────────────────────────────────────────

enum Dir { CENTER, UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT };

const char* DIR_LABEL[] = {
  "·", "↑  UP", "↗  UP-RIGHT", "→  RIGHT", "↘  DOWN-RIGHT",
  "↓  DOWN", "↙  DOWN-LEFT", "←  LEFT", "↖  UP-LEFT",
};

Preferences prefs;
int  centerX, centerY, threshold;   // centerX = center of VRX_PIN, centerY = of VRY_PIN
Dir  prevDir = CENTER;
bool prevSW  = true;

// ─── orientation ──────────────────────────────────────────────────────────────
//
// Takes already-centered native deltas (rvx = VRx-centerX, rvy = VRy-centerY) and
// maps them into the world frame (+x right, +y down). The table is anchored to the
// hardware-verified case (ROT_90_CCW: dx=+rvy, dy=-rvx); the other three are derived
// as a consistent rotation (each one turns 90° from the previous).
// Only ROT_90_CCW is physically confirmed — verify the others when remounting.
void applyMount(int rvx, int rvy, float &dx, float &dy) {
  switch (JOY_MOUNT) {
    case ROT_0:      dx = -rvx; dy = -rvy; break;
    case ROT_90_CW:  dx = -rvy; dy =  rvx; break;
    case ROT_180:    dx =  rvx; dy =  rvy; break;
    case ROT_90_CCW: dx =  rvy; dy = -rvx; break;   // ✅ verified on test board
  }
}

// ─── calibration ──────────────────────────────────────────────────────────────

void computeThreshold() {
  int minRange = min({ centerX, centerY, 4095 - centerX, 4095 - centerY });
  threshold = (minRange * THRESHOLD_PCT) / 100;
}

void runCalibration() {
  Serial.println("Calibrating — don't touch the joystick...");
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
  Serial.printf("Center: VRx=%d  VRy=%d  | threshold: ±%d (%d%%) — saved to flash\n",
                centerX, centerY, threshold, THRESHOLD_PCT);
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
    Serial.printf("Calibration from memory: VRx=%d  VRy=%d  | threshold: ±%d\n",
                  centerX, centerY, threshold);
  } else {
    Serial.println("No saved calibration.");
    runCalibration();
  }
}

// ─── direction by angle ───────────────────────────────────────────────────────
//
// KNOWN ISSUE (jittery diagonals): the 8 sectors are hard 45° boundaries, so when
// the lever sits near a boundary (e.g. DOWN-LEFT) tiny ADC noise flips the reading
// between the diagonal and the adjacent cardinals (LEFT / DOWN), and momentary dips
// below `threshold` drop it to CENTER. Output looks like:  DOWN-LEFT · LEFT · DOWN ...
// Fine for a raw test, but NOT good enough for menu/note selection.
//
// TODO (production firmware — smooth selection): add either
//   - angular hysteresis: keep the current Dir until the angle moves well past the
//     boundary (e.g. ±5–8° overshoot) before switching, so it doesn't chatter; and/or
//   - a dwell/debounce: only commit a new Dir after it holds stable for N ms; and/or
//   - a larger center deadzone for selection mode to kill the CENTER flicker.
// For continuous use (pitch bend / expression) prefer the raw dx/dy from applyMount()
// over this 8-way bucketing entirely.
Dir getDir(int rawVRx, int rawVRy) {
  float dx, dy;
  applyMount(rawVRx - centerX, rawVRy - centerY, dx, dy);   // world frame

  float mag = sqrtf(dx * dx + dy * dy);
  if (mag < threshold) return CENTER;

  float a = atan2f(dy, dx) * 180.0f / (float)M_PI;

  if (a >= -22.5f  && a <  22.5f)  return RIGHT;
  if (a >=  22.5f  && a <  67.5f)  return DOWN_RIGHT;
  if (a >=  67.5f  && a < 112.5f)  return DOWN;
  if (a >= 112.5f  && a < 157.5f)  return DOWN_LEFT;
  if (a >= -67.5f  && a < -22.5f)  return UP_RIGHT;
  if (a >= -112.5f && a < -67.5f)  return UP;
  if (a >= -157.5f && a < -112.5f) return UP_LEFT;
  return LEFT;
}

// ─── setup / loop ─────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(115200);
  pinMode(JOY_SW_PIN, INPUT_PULLUP);
  analogReadResolution(12);
  delay(500);

  Serial.println("=== KY-023 joystick test ===");
  Serial.println("(hold SW on power-up to recalibrate)");

  if (digitalRead(JOY_SW_PIN) == LOW) {
    Serial.println("SW detected — forcing recalibration...");
    runCalibration();
  } else {
    loadOrCalibrate();
  }

  Serial.println("Ready.\n");
}

void loop() {
  Dir  dir = getDir(analogRead(VRX_PIN), analogRead(VRY_PIN));
  bool sw  = digitalRead(JOY_SW_PIN);

  if (dir != prevDir) {
    Serial.println(DIR_LABEL[dir]);
    prevDir = dir;
  }
  if (sw != prevSW) {
    if (!sw) Serial.println("● PRESSED");
    prevSW = sw;
  }

  delay(20);
}
