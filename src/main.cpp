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

void randomSides() {
  for (int i = 0; i < numSides; i++) {
    int randomSeed = esp_random();
    int rand = random8(randomSeed);
    CHSV randomColor = CHSV(rand, 255, 255);
    for (int ii = 0; ii < numSides; ii++) {
      int offset = (i * sideLength) + ii;
      leds[offset] = randomColor;
    }
  }
}

void randomLegs() {
  for (int i = 0; i < numberOfLegs; i++) {
    int rand = random8();
    CHSV randomColor = CHSV(rand, 255, 255);
    for (int ii = 0; ii < legLength; ii++) {
      int offset = (i * legLength) + ii;
      leds[offset] = randomColor;
    }
  }
}

void santaSides() {
  for (int i = 0; i < numSides; i++) {
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
    std::copy(newArray + 0, newArray + legLength,
              leds + (NUMPIXELS - legLength));
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

namespace Animations {

  namespace Perimeter {
    int currentIndex;
    int perimeterHue;
    void setup() {
      currentIndex = 0;
      perimeterHue = 0;
    }

    void run() {
      if (timer.hasElapsedWithReset(100)) {
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
        FastLED.show();
      }
    }
  } // namespace Perimeter

  namespace RotateRandomLegs {
    bool setupComplete = false;
    void setup() {
      randomLegs();
      mut::shiftBySideLength();
      setupComplete = true;
    };
    void run() {
      if (timer.hasElapsedWithReset(250)) {
        Serial.println("Rotate");
        if (!setupComplete) {
          setup();
        }
        mut::rotateLegs();
        FastLED.show();
      }
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
      if (timer.hasElapsedWithReset(100)) {
        Serial.println("Santa");
        if (!setupComplete) {
          setup();
        }
        if (counter % sideLength == 0) {
          delay(500);
        }
        mut::shiftByOne();
        counter++;
        FastLED.show();
      }
    }
  } // namespace SantaSlide

  namespace Stars {
    int lastPosition = 0;
    int numPixelsBetweenStars = 4;
    int delay;
    void setup() {
      Serial.println("Stars Setup");
      delay = 200;
    }
    void run() {
      if (timer.hasElapsedWithReset(delay)) {
        if (delay > 0)
          delay = delay - 1;
        Serial.println("Stars");
        lastPosition++;
        if (lastPosition > numPixelsBetweenStars)
          lastPosition = 0;

        fill_solid(leds, NUMPIXELS, CRGB::Black);
        for (int dot = lastPosition; dot < NUMPIXELS;
             dot += numPixelsBetweenStars + 1) {
          leds[dot] = CRGB::Coral;
        }
        FastLED.show();
      }
    }
  } // namespace Stars

  namespace Crossfade {
    int hue;
    int delay;
    void setup() {
      hue = 0;
      delay = 150;
    }
    void run() {
      if (timer.hasElapsedWithReset(delay)) {
        if (delay > 0)
          delay = delay - 1;
        fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
        FastLED.show();
      }
    }
  } // namespace Crossfade

  namespace Gradient {
    int counter;
    void setup() { counter = 0; }
    void run() {
      if (timer.hasElapsedWithReset(10)) {
        for (int i = 0; i < NUMPIXELS; i++) {
          leds[i] = CHSV((i * 2.5) + counter, 255, 255);
        }
        counter = counter + 1;
        FastLED.show();
      }
    }
  } // namespace Gradient
} // namespace Animations

typedef void (*Pattern)();
typedef void (*Setup)();
typedef Pattern PatternList[];
typedef struct {
  Pattern run;
  Pattern setup;
  uint16_t mTime;
} PatternAndTime;
typedef PatternAndTime PatternAndTimeList[];

// *****************      Playlist settings       **************** //
const PatternAndTimeList gPlaylist = {
    {Animations::Gradient::run, Animations::Gradient::setup, 10},
    {Animations::SantaSlide::run, Animations::SantaSlide::setup, 10},
    {Animations::Crossfade::run, Animations::Crossfade::setup, 20},
    {Animations::Perimeter::run, Animations::Perimeter::setup, 20},
    {Animations::Stars::run, Animations::Stars::setup, 20},
    {Animations::RotateRandomLegs::run, Animations::RotateRandomLegs::setup,
     10},
};
// *****************      Playlist settings       **************** //

uint8_t gCurrentTrackNumber = 0;
bool gLoopPlaylist = true;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern() {
  gCurrentTrackNumber = gCurrentTrackNumber + 1;

  if (gCurrentTrackNumber == ARRAY_SIZE(gPlaylist)) {
    gCurrentTrackNumber = 0;
  }
  gPlaylist[gCurrentTrackNumber].setup();
}

void loop() {
  using namespace Animations;
  gPlaylist[gCurrentTrackNumber].run();
  {
    EVERY_N_SECONDS_I(patternTimer, gPlaylist[gCurrentTrackNumber].mTime) {
      nextPattern();

      patternTimer.setPeriod(gPlaylist[gCurrentTrackNumber].mTime);
    }
  }
}