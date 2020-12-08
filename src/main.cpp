#include <Arduino.h>

#include "config.h"
#define FASTLED_INTERNAL
#include <SPI.h>

#include "DebugLog.h"
#include "FastLED.h"
#include "MilliTimer.h"

MilliTimer timer{};

CRGB leds[NUMPIXELS];

int legLength = NUMPIXELS / 5;
int sideLength = legLength / 2;
int numSides = 10;

uint8_t hue = 0;
void setup() {
  FastLED.addLeds<WS2811, DATAPIN, GRB>(leds, NUMPIXELS);
  Serial.begin(115200);
}

void loop() {
  if (timer.hasElapsedWithReset(100)) {
    // fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
    for (int i = 0; i < numSides; i++) {
      for (int ii = 0; ii < sideLength; ii++) {
        int offset = (i * sideLength) + ii;
        if (i % 2 == 0) {
          leds[offset].setRGB(0, 255, 0);
        } else {
          leds[offset].setRGB(255,0,  0);
        }
      }
    }
    FastLED.show();
  }
}