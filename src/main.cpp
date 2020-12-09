#include <Arduino.h>

#include "config.h"
#define FASTLED_INTERNAL
#include <SPI.h>

#include "DebugLog.h"
#include "FastLED.h"
#include "MilliTimer.h"
#include <algorithm>

MilliTimer timer{};

CRGB leds[NUMPIXELS];

const int numberOfLegs = 5;
const int legLength = NUMPIXELS / numberOfLegs;
const int sideLength = legLength / 2;
const int numSides = 10;

uint8_t hue = 0;

void setup() {
  FastLED.addLeds<WS2811, DATAPIN, RGB>(leds, NUMPIXELS);
  Serial.begin(115200);
}

void displaySides() {
  for (int i = 0; i < numSides; i++) {
    for (int ii = 0; ii < sideLength; ii++) {
      int offset = (i * sideLength) + ii;
      if (i % 2 == 0) {
        leds[offset].setRGB(0, 255, 0);
      } else {
        leds[offset].setRGB(255, 0, 0);
      }
    }
  }
}

CRGB side[sideLength] = {CRGB::Black};

void reverseSide(CRGB pixels[sideLength]) {
  std::reverse(pixels, pixels + sideLength);
}

void eachSide(CRGB pixels[sideLength], bool reverse = false) {
  for (int i = 0; i < numSides; i++) {
    if (reverse) {
      reverseSide(pixels);
    };
    for (int ii = 0; ii < sideLength; ii++) {
      int offset = (i * sideLength) + ii;
      leds[offset] = pixels[ii];
    }
  }
}

void setAll(CRGB pixels[], int length, CRGB color) {
  for (int i = 0; i < length; i++) {
    pixels[i] = color;
  }
}

int currentIndex = 0;
int perimeterHue = 0;
void perimeter() {
  currentIndex++;
  if (currentIndex >= sideLength) {
    currentIndex = 0;
  }
  perimeterHue++;
  if (perimeterHue > 255)
    perimeterHue = 0;
  setAll(side, sideLength, CRGB::Black);
  side[currentIndex + 0] = CRGB::LightYellow;
  side[currentIndex + 1] = CHSV(perimeterHue, 255, 255);
  side[currentIndex + 2] = CRGB::LightYellow;
  eachSide(side, true);
}

void crossfade() { fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255)); }

int counter = 0;
void gradient() {
  for (int i = 0; i < NUMPIXELS; i++) {
    leds[i] = CHSV((i * 2.5) + counter, 255, 255);
  }
  counter = counter + 1;
}

namespace RotateLegs {
void setup() {
  CRGB legs[NUMPIXELS];
  for (int i = 0; i < numberOfLegs; i++) {}
  // legLength
};
void run() {}
} // namespace RotateLegs

void loop() {
  if (timer.hasElapsedWithReset(120)) {
    perimeter();
    // crossfade();
    // gradient();
    FastLED.show();
  }
}