#pragma once
#include <cstdint>

#define ANIM_DEBUG false

const int NUMPIXELS = 70;

// Here we define boards as integers to be used in `platform.ini` passed build
// flags. This way we can use the easier to reference varaiable name to keep the
// code readable when conditionally compiling blocks of code.

const int CLOCKPIN = 4;
const int DATAPIN = 5;
