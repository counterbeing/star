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

void randomSides() {
  for (int i = 0; i < numSides; i++) {
    int randomSeed = esp_random();
    int rand = random8(randomSeed);
    Serial.println(rand);
    CHSV randomColor = CHSV(rand, 255, 255);
    for (int ii = 0; ii < numSides; ii++) {
      int offset = (i * sideLength) + ii;
      leds[offset] = randomColor;
    }
  }
}

void randomLegs() {
  for (int i = 0; i < numberOfLegs; i++) {
    // int randomSeed = esp_random();
    int rand = random8();
    Serial.println(rand);
    CHSV randomColor = CHSV(rand, 255, 255);
    for (int ii = 0; ii < legLength; ii++) {
      int offset = (i * legLength) + ii;
      leds[offset] = randomColor;
    }
  }
}

void santaSides() {
  for (int i = 0; i < numSides; i++) {
    int randomSeed = esp_random();
    int rand = random8(randomSeed);
    Serial.println(rand);
    CRGB randomColor = i % 2 == 0 ? CRGB::White : CRGB::Red;
    for (int ii = 0; ii < numSides; ii++) {
      int offset = (i * sideLength) + ii;
      leds[offset] = randomColor;
    }
  }
}

namespace mut {
void rotateLegs() {
  CRGB newArray[NUMPIXELS];
  std::copy(std::begin(leds), std::end(leds), std::begin(newArray));
  std::copy(newArray + 0, newArray + legLength, leds + (NUMPIXELS - legLength));
  std::copy(newArray + legLength, newArray + NUMPIXELS, leds);
}

void shiftBySideLength() {
  CRGB newArray[NUMPIXELS];
  std::copy(std::begin(leds), std::end(leds), std::begin(newArray));
  std::copy(newArray + 0, newArray + sideLength,
            leds + (NUMPIXELS - sideLength));
  std::copy(newArray + sideLength, newArray + NUMPIXELS, leds);
}

void shiftByOne() {
  CRGB newArray[NUMPIXELS];
  std::copy(std::begin(leds), std::end(leds), std::begin(newArray));
  std::copy(newArray + 0, newArray + 1, leds + (NUMPIXELS - 1));
  std::copy(newArray + 1, newArray + NUMPIXELS, leds);
}
} // namespace mut

namespace RotateRandomLegs {
bool setupComplete = false;
void setup() {
  randomLegs();
  mut::shiftBySideLength();
  setupComplete = true;
};
void run() {
  if (!setupComplete) {
    setup();
  }
  mut::rotateLegs();
}
} // namespace RotateRandomLegs

namespace SantaSlide {
bool setupComplete = false;
int counter = 0;
void setup() {
  santaSides();
  setupComplete = true;
};
void run() {
  if (!setupComplete) {
    setup();
  }
  if (counter % sideLength == 0) {
    delay(500);
  }
  mut::shiftByOne();
  counter++;
}
} // namespace SantaSlide

void loop() {

  if (timer.hasElapsedWithReset(300)) {
    // RotateRandomLegs::run();
    // SantaSlide::run();
    perimeter();
    // crossfade();
    // gradient();
    FastLED.show();
  }
}