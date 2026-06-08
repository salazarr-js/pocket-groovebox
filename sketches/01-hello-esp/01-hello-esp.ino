// Onboard addressable RGB LED (WS2812) on GPIO38 (confirmed working with NeoPixel).
// Use our own macro name — do NOT reuse RGB_BUILTIN, which the board profile already
// defines (with a special value), so an #ifndef guard would silently ignore our pin.
#define LED_PIN 38

void setup() {
  Serial.begin(115200);
  delay(2000); // let native USB serial come up
  Serial.println("\nPocket Groovebox — ESP32-S3 test");
  Serial.printf("Chip:  %s (%d cores)\n", ESP.getChipModel(), ESP.getChipCores());
  Serial.printf("Flash: %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("PSRAM: %u MB\n", ESP.getPsramSize() / (1024 * 1024));
  Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());
  rgbLedWrite(LED_PIN, 0, 0, 0);  // clear LED on boot for a known state
}

void loop() {
  // This LED uses RGB byte order, while rgbLedWrite() assumes GRB — so the red and
  // green channels are swapped. We swap the r/g arguments here to compensate (blue is
  // the 3rd byte in both orders, so it's already correct).
  rgbLedWrite(LED_PIN, 0, 32, 0); // red
  Serial.println("alive... red");
  delay(1000);

  rgbLedWrite(LED_PIN, 32, 0, 0); // green
  Serial.println("alive... green");
  delay(1000);

  rgbLedWrite(LED_PIN, 0, 0, 32); // blue
  Serial.println("alive... blue");
  delay(1000);
}
