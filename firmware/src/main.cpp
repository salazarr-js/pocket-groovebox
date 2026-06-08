#include <Arduino.h>

#define LED_PIN 38

void setup() {
  Serial.begin(115200);
  delay(2000);

  Serial.println("\nPocket Groovebox — PlatformIO board test");
  Serial.printf("Chip:      %s (%d cores @ %lu MHz)\n", ESP.getChipModel(), ESP.getChipCores(), ESP.getCpuFreqMHz());
  Serial.printf("Flash:     %u MB\n", ESP.getFlashChipSize() / (1024 * 1024));
  Serial.printf("PSRAM:     %u MB\n", ESP.getPsramSize() / (1024 * 1024));
  Serial.printf("Free heap: %u bytes\n", ESP.getFreeHeap());

  // Verify PSRAM is actually accessible — allocates 1 MB from PSRAM
  void* buf = ps_malloc(1024 * 1024);
  if (buf) {
    Serial.println("PSRAM:     OK (1 MB allocated)");
    free(buf);
  } else {
    Serial.println("PSRAM:     FAIL — check opi_opi memory_type in platformio.ini");
  }

  // LED color order on this board is RGB; neopixelWrite() sends GRB,
  // so r and g arguments are swapped to compensate.
  neopixelWrite(LED_PIN, 0, 0, 0);
}

void loop() {
  neopixelWrite(LED_PIN, 0, 32, 0); Serial.println("red");   delay(1000);
  neopixelWrite(LED_PIN, 32, 0, 0); Serial.println("green"); delay(1000);
  neopixelWrite(LED_PIN, 0, 0, 32); Serial.println("blue");  delay(1000);
}
