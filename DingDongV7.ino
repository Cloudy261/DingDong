#include "DingDong.h"

EMPTY_INTERRUPT(PCINT0_vect);

  DingDong dd;

void setup() {
    dd.setup();
}

void loop() {
  dd.routine();
}