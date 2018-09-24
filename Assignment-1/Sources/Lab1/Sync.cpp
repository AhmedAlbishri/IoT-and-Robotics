#include "Sync.h"
#include <Arduino.h>

Sync::Sync() {
	reset();
}

Sync::Sync(unsigned long start) {
  previous = start;
}

void Sync::reset() {
	previous = 0;
}

bool Sync::elapsed(unsigned long timeout) {
  unsigned long now = millis();
  return now - previous >= timeout ? (previous = now) : false;
}
