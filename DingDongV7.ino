#include "DingDong.h"

EMPTY_INTERRUPT(PCINT0_vect);

  DingDong dd;

void setup() {
    //dd.resetEEPROM();
    dd.setup();
}

void loop() {
  dd.routine();
} 
