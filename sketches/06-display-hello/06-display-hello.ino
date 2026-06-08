// 06-display-hello — first light for the ST7789 1.9" 170x320 IPS display.
// Runs a color test (red/green/blue fills) then draws a little UI: title, divider,
// some labeled values, and a live counter — to confirm SPI, the panel offset, and
// colors before building the real UI.
//
// Library: "GFX Library for Arduino" (Arduino_GFX, by moononournation) — install via
// Library Manager. Chosen over TFT_eSPI because pins are set here in code, no config
// file to edit. Handles the 170x320 column offset for the ST7789. See docs/hardware/modules/st7789.md.
//
// Pins (docs/hardware/wiring.md), GPIO 9-14 assigned in the screen's own header order
// (SCL·SDA·RES·DC·CS·BLK) so the wires run straight across:
//   SCL=9, SDA=10, RES=11, DC=12, CS=13, BLK=14.  VCC=3V3, GND=GND. Write-only SPI (no MISO).

#include <Arduino_GFX_Library.h>

// Arduino_GFX names its RGB565 colors with an RGB565_ prefix (not the bare RED/BLACK of
// Adafruit_GFX). Alias the short names so the drawing code below stays readable.
#define BLACK   RGB565_BLACK
#define WHITE   RGB565_WHITE
#define RED     RGB565_RED
#define GREEN   RGB565_GREEN
#define BLUE    RGB565_BLUE
#define CYAN    RGB565_CYAN
#define MAGENTA RGB565_MAGENTA
#define YELLOW  RGB565_YELLOW
#define ORANGE  RGB565_ORANGE

constexpr int PIN_SCLK = 9;    // SCL
constexpr int PIN_MOSI = 10;   // SDA
constexpr int PIN_RST  = 11;   // RES
constexpr int PIN_DC   = 12;   // DC
constexpr int PIN_CS   = 13;   // CS
constexpr int PIN_BL   = 14;   // BLK — backlight, drive HIGH to turn on

// SPI bus (DC, CS, SCK, MOSI, MISO=not used).
Arduino_DataBus *bus = new Arduino_ESP32SPI(PIN_DC, PIN_CS, PIN_SCLK, PIN_MOSI, GFX_NOT_DEFINED);

// ST7789, IPS, 170x320. The last four numbers are the panel offsets: the 170-wide panel
// sits 35 columns into the controller's 240-wide RAM, so col offset = 35 (row offset = 0).
// If the image looks shifted/wrapped, that's the offset to adjust.
Arduino_GFX *gfx = new Arduino_ST7789(bus, PIN_RST, 0 /*rotation*/, true /*IPS*/,
                                      170, 320, 35, 0, 35, 0);

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(PIN_BL, OUTPUT);
  digitalWrite(PIN_BL, HIGH);          // backlight on

  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed! — check wiring (SCLK/MOSI/CS/DC/RST) and VCC");
    while (true) delay(1000);
  }
  Serial.println("\nDisplay up — color test, then UI");

  // Color test: if red/green/blue come out wrong or negative, the panel may need
  // inversion toggled — try gfx->invertDisplay(false) (IPS usually wants it on).
  gfx->fillScreen(RED);    delay(500);
  gfx->fillScreen(GREEN);  delay(500);
  gfx->fillScreen(BLUE);   delay(500);

  // Static UI scaffold.
  gfx->fillScreen(BLACK);
  gfx->setTextColor(WHITE);
  gfx->setTextSize(2);
  gfx->setCursor(6, 8);
  gfx->println("Pocket");
  gfx->setCursor(6, 28);
  gfx->println("Groovebox");

  gfx->drawFastHLine(0, 52, gfx->width(), ORANGE);   // accent divider

  gfx->setTextSize(1);
  gfx->setCursor(6, 64);
  gfx->setTextColor(CYAN);
  gfx->println("display: ST7789 170x320");
  gfx->setCursor(6, 78);
  gfx->println("count:");

  // A couple of shapes to eyeball geometry/offset.
  gfx->drawRect(4, 100, gfx->width() - 8, 40, WHITE);
  gfx->fillRoundRect(10, 108, 60, 24, 4, MAGENTA);
}

void loop() {
  // Live value — proves the screen updates, not just a static splash.
  static uint32_t count = 0;
  gfx->fillRect(48, 78, 60, 10, BLACK);   // clear old number
  gfx->setTextColor(YELLOW);
  gfx->setTextSize(1);
  gfx->setCursor(48, 78);
  gfx->print(count++);
  delay(200);
}
