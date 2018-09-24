#include "Sync.h"
#include <Arduino.h>

Sync::Sync() {
	reset();
}

void Sync::reset() {
	previous = 0;
}

bool Sync::elapsed(unsigned long timeout) {
	return elapsedMicroseconds(timeout * 1000);
}

bool Sync::elapsedMicroseconds(unsigned long timeout) {
	unsigned long now = micros();
	return now - previous >= timeout ? (previous = now) : false;
}
