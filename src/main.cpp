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
const int numSides = numberOfLegs * 2;

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

void alternateSides(CRGB color1, CRGB color2) {
  for (int i = 0; i < numSides; i++) {
    CRGB randomColor = i % 2 == 0 ? color1 : color2;
    for (int ii = 0; ii < sideLength; ii++) {
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

  void shiftStrandByOne() {
    CRGB newArray[NUMPIXELS];
    std::copy(std::begin(leds), std::end(leds), std::begin(newArray));
    std::copy(newArray + 0, newArray + 1, leds + (NUMPIXELS - 1));
    std::copy(newArray + 1, newArray + NUMPIXELS, leds);
  }

  void shiftSideByOne(CRGB arr[], int arrLength) {
    CRGB newArray[arrLength];
    std::copy(arr, arr + arrLength, newArray);
    std::copy(newArray + 0, newArray + 1, arr + (arrLength - 1));
    std::copy(newArray + 1, newArray + arrLength, arr);
  }

  void shiftSideByOneBlackout(CRGB arr[], int arrLength) {
    CRGB newArray[arrLength];
    std::copy(arr, arr + arrLength, newArray);
    std::copy(newArray, newArray + arrLength - 1, arr + 1);
    arr[0] = CRGB::Black;
  }
} // namespace mut

namespace Animations {

  namespace Perimeter {
    uint8_t currentIndex;
    uint16_t perimeterHue;
    CRGB side[sideLength];

    void setup() {
      side[sideLength] = {CRGB::Black};
      currentIndex = 0;
      perimeterHue = 0;
    }

    void run() {
      if (timer.hasElapsedWithReset(100)) {
        currentIndex++;
        if (currentIndex >= sideLength - 2)
          currentIndex = 0;

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

  namespace StarFish {
    void setup() {
      randomLegs();
      mut::shiftBySideLength();
    };
    void run() {
      if (timer.hasElapsedWithReset(250)) {
        mut::rotateLegs();
        FastLED.show();
      }
    }
  } // namespace StarFish

  namespace SantaSlide {
    int counter;
    void setup() {
      counter = 0;
      CRGB color;
      uint32_t colors[] = {CRGB::Red, CRGB::Green, CRGB::Gold};
      uint8_t colorIndex = random(0, 3);
      color = colors[colorIndex];
      alternateSides(CRGB::White, color);
    };

    void run() {
      if (timer.hasElapsedWithReset(100)) {
        if (counter % sideLength == 0) {
          delay(500);
        }
        mut::shiftStrandByOne();
        counter++;
        FastLED.show();
      }
    }
  } // namespace SantaSlide

  namespace MeteorShower {
    int lastPosition = 0;
    int numPixelsBetweenStars = 4;
    int delay;
    void setup() { delay = 200; }
    void run() {
      if (timer.hasElapsedWithReset(delay)) {
        if (delay > 40)
          delay = delay - 1;
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
  } // namespace MeteorShower

  namespace Crossfade {
    uint8_t hue;
    int delay;
    void setup() {
      hue = random(0, 255);
      delay = 100;
    }
    void run() {

      if (timer.hasElapsedWithReset(delay)) {
        // Serial.print("HUE: ");
        // Serial.println(hue);
        // Serial.println(delay);
        if (hue > 255)
          hue = 0;
        if (delay > 20) {
          delay = delay - 1;
        }
        fill_solid(leds, NUMPIXELS, CHSV(hue++, 255, 255));
        FastLED.show();
      }
    }
  } // namespace Crossfade

  namespace RotateGradient {
    int counter;
    void setup() { counter = 0; }
    void run() {
      if (timer.hasElapsedWithReset(30)) {
        for (int i = 0; i < NUMPIXELS; i++) {
          leds[i] = CHSV((i * 2.5) + counter, 255, 255);
        }
        counter = counter + 1;
        FastLED.show();
      }
    }
  } // namespace RotateGradient

  namespace RainbowTunnel {
    uint8_t hue;
    CRGB side[sideLength];

    void setup() {
      side[sideLength] = {CRGB::Black};
      hue = random(0, 255);
    }

    void run() {
      if (timer.hasElapsedWithReset(1)) {
        if (hue > 255)
          hue = 0;
        int hueStart = hue;
        for (int i = 0; i < sideLength; i++) {
          Serial.println(hueStart);
          side[i] = CHSV(hueStart += 20, 255, 255);
        }
        eachSide(side, true);
        FastLED.show();
        hue++;
      }
    }
  } // namespace RainbowTunnel

  namespace WarpSpeedSleigh {
    CRGB side[sideLength];
    uint8_t counter = 0;
    uint8_t gap;
    uint32_t colors[] = {CRGB::White, CRGB::Red, CRGB::Green, CRGB::Gold};
    void setup() {
      side[sideLength] = {CRGB::Black};
      gap = random(5, 15);
    }

    void run() {
      if (timer.hasElapsedWithReset(80)) {
        if (counter >= gap) {
          gap = random(3, sideLength);
          uint8_t colorIndex = random(0, 4);
          side[0] = colors[colorIndex];
          counter = 0;
        } else {
          counter++;
        }
        mut::shiftSideByOneBlackout(side, sideLength);

        eachSide(side, true);
        FastLED.show();
      }
    }
  } // namespace WarpSpeedSleigh
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
    {Animations::RainbowTunnel::run, Animations::RainbowTunnel::setup, 30},
    {Animations::WarpSpeedSleigh::run, Animations::WarpSpeedSleigh::setup, 20},
    {Animations::RotateGradient::run, Animations::RotateGradient::setup, 20},
    {Animations::SantaSlide::run, Animations::SantaSlide::setup, 20},
    {Animations::Crossfade::run, Animations::Crossfade::setup, 20},
    {Animations::Perimeter::run, Animations::Perimeter::setup, 20},
    {Animations::StarFish::run, Animations::StarFish::setup, 20},
    {Animations::MeteorShower::run, Animations::MeteorShower::setup, 20},
};
// *****************      Playlist settings       **************** //

uint8_t gCurrentTrackNumber = 0;
bool gLoopPlaylist = true;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
void runSetupForAnimation() { gPlaylist[gCurrentTrackNumber].setup(); }

void nextPattern() {
  gCurrentTrackNumber = gCurrentTrackNumber + 1;

  if (gCurrentTrackNumber == ARRAY_SIZE(gPlaylist)) {
    gCurrentTrackNumber = 0;
  }
  runSetupForAnimation();
}

void setup() {
  Serial.println("running setup");
  FastLED.addLeds<WS2811, DATAPIN, RGB>(leds, NUMPIXELS);
  Serial.begin(115200);
  FastLED.clear();
  runSetupForAnimation();
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