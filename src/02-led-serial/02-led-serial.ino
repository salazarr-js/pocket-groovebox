// LED color via serial — type a letter in the Serial Monitor to set the onboard RGB LED.
//   r = red   g = green   b = blue   w = white   o = off
//
// Uses the core's built-in rgbLedWrite() — no library to install.
// Onboard WS2812-type LED on GPIO38. This board is RGB byte order while rgbLedWrite()
// transmits GRB, so red/green come out swapped — setColor() swaps them back.
// (Alternative: Adafruit NeoPixel with NEO_RGB — see docs/development.md.)

#define LED_PIN 38

// Set the LED to a logical (r, g, b). rgbLedWrite() sends GRB; this board is RGB, so we
// pass green/red swapped to land on the right color.
void setColor(uint8_t r, uint8_t g, uint8_t b, const char *name) {
  rgbLedWrite(LED_PIN, g, r, b);
  Serial.printf("LED: %s\n", name);
}

void setup() {
  Serial.begin(115200);
  delay(500);
  rgbLedWrite(LED_PIN, 0, 0, 0);  // clear LED on boot for a known state
  Serial.println("\nLED serial test — send a letter: r g b y c m w o");
}

void loop() {
  if (!Serial.available()) return;
  char c = tolower(Serial.read());  // accept upper- or lowercase
  const uint8_t V = 64;  // brightness (0–255); WS2812s are bright, keep it low
  switch (c) {
    case 'r': setColor(V, 0, 0, "red");      break;
    case 'g': setColor(0, V, 0, "green");    break;
    case 'b': setColor(0, 0, V, "blue");     break;
    case 'y': setColor(V, V, 0, "yellow");   break;
    case 'c': setColor(0, V, V, "cyan");     break;
    case 'm': setColor(V, 0, V, "magenta");  break;
    case 'w': setColor(V, V, V, "white");    break;
    case 'o': setColor(0, 0, 0, "off");      break;
    case '\n': case '\r': break;  // ignore line endings
    default: Serial.printf("? '%c' — use r / g / b / y / c / m / w / o\n", c); break;
  }
}
