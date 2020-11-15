#include "DingDong.h"

EMPTY_INTERRUPT(PCINT0_vect);

  DingDong dd;

void setup() {
    dd.setup();
    //dd.resetEEPROM();
}

void loop() {
  dd.routine();
} 
