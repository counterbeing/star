#pragma once
#include "DebugLog.h"
#include "config.h"
#include <Bounce2.h>

enum ClickEvent : uint8_t { CLICK, LONG_CLICK, DOUBLE_CLICK, OTHER };

class LSButton {
 private:
  static const unsigned int DOUBLE_CLICK_MAX_GAP =
      250; // max ms between clicks for single event
  static const unsigned int SHORT_CLICK_MAX_DURATION = 250;
  static const unsigned int LONG_CLICK_MIN_DURATION = 1750;
  ClickEvent latestClickEvent = ClickEvent::OTHER;
  bool possibleDoubleClick = false;
  bool possibleLongClick = false;
  bool clickEventOccurredFlag = false;

  void setLatestClickEvent(ClickEvent clickEvent) {
    latestClickEvent = clickEvent;
    clickEventOccurredFlag = true;
    if (KNOB_DEBUG) {
      switch (clickEvent) {
        case ClickEvent::CLICK:
          debugLog("Event occurred: CLICK");
          break;
        case ClickEvent::LONG_CLICK:
          debugLog("Event occurred: LONG_CLICK");
          break;
        case ClickEvent::DOUBLE_CLICK:
          debugLog("Event occurred: DOUBLE_CLICK");
          break;
        default:
          debugLog("Event occurred: OTHER");
          break;
      }
    }
  }

 public:
  Bounce buttonDebouncer;
  LSButton() {
    Serial.println("INIT BUTTON ===========================");
    buttonDebouncer = Bounce();
  };
  ClickEvent getLatestClickEvent() { return latestClickEvent; }

  bool hasClickEventOccurred() { return clickEventOccurredFlag; }

  bool hasClickEventOccurred(const ClickEvent clickEvent) {
    return clickEventOccurredFlag && clickEvent == latestClickEvent;
  }

  void checkButton() {
    clickEventOccurredFlag = false;

    if (buttonDebouncer.rose()) {
      unsigned long buttonDownDuration = buttonDebouncer.previousDuration();
      if (KNOB_DEBUG)
        debugLog("button up, down duration = ", buttonDownDuration);

      possibleLongClick = false;

      if (buttonDownDuration <= SHORT_CLICK_MAX_DURATION) {
        if (possibleDoubleClick) {
          possibleDoubleClick = false;
          setLatestClickEvent(ClickEvent::DOUBLE_CLICK);
          return;
        }
        possibleDoubleClick = true;
      } else {
        possibleDoubleClick = false;
      }

    }

    else if (buttonDebouncer.fell()) {
      if (KNOB_DEBUG)
        debugLog("button down, up duration = ",
                 buttonDebouncer.previousDuration());
      possibleLongClick = true;
    }

    else { // no button state change since neither rose nor fell
      if (possibleLongClick &&
          buttonDebouncer.duration() >= LONG_CLICK_MIN_DURATION) {
        setLatestClickEvent(ClickEvent::LONG_CLICK);
        possibleLongClick = false;
        return;
      }

      if (possibleDoubleClick &&
          buttonDebouncer.duration() > DOUBLE_CLICK_MAX_GAP) {
        setLatestClickEvent(ClickEvent::CLICK);
        possibleDoubleClick = false;
        return;
      }
    }
  }

  void setup() {
    buttonDebouncer.attach(buttonPin, INPUT_PULLUP);
    buttonDebouncer.interval(5);
  }

  void loop() { buttonDebouncer.update(); }
};