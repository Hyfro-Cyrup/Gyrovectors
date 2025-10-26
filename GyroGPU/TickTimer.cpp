#include "TickTimer.h"

#include <chrono>


TickTimer::TickTimer(double seconds_per_tick) :
	previousTime{ std::chrono::high_resolution_clock::now() },
	elapsedTime{ 0.0 },
	secondsPerTick{ seconds_per_tick } {
}
TickTimer::TickTimer(int ticks_per_second) : TickTimer(1.0 / ticks_per_second) {}
TickTimer::TickTimer() : TickTimer(30) {}

void TickTimer::update() {
	auto currentTime = std::chrono::high_resolution_clock::now();
	elapsedTime += std::chrono::duration_cast<std::chrono::duration<double>>(currentTime - previousTime).count();
	previousTime = currentTime;
}

bool TickTimer::isTickReady() const {
	return elapsedTime > secondsPerTick;
}

void TickTimer::decrement() {
	elapsedTime -= secondsPerTick;
}