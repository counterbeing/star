#include "config.h"
#include <Arduino.h>
#define FASTLED_INTERNAL
#include "DebugLog.h"
#include "FastLED.h"
#include "MilliTimer.h"
#include <SPI.h>
 
MilliTimer timer{};

CRGB leds[NUMPIXELS];


uint8_t hue = 0;
void setup() {
  FastLED.addLeds<WS2811, DATAPIN, GRB>(leds, NUMPIXELS);
  Serial.begin(115200);

}

void loop() {
  if (timer.hasElapsedWithReset(100)) {
    fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
    FastLED.show();
  }
}