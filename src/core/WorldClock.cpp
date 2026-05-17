#include "core/WorldClock.hpp"

#include <cmath>

void WorldClock::SetTimeOfDayHours(double hours) {
    timeOfDayHours_ = std::fmod(hours, 24.0);
    if (timeOfDayHours_ < 0.0) {
        timeOfDayHours_ += 24.0;
    }
}

void WorldClock::Advance(double fixedDeltaSeconds, double worldHoursPerRealSecond) {
    SetTimeOfDayHours(timeOfDayHours_ + (fixedDeltaSeconds * worldHoursPerRealSecond));
}

double WorldClock::TimeOfDayHours() const {
    return timeOfDayHours_;
}

double WorldClock::NormalizedDayProgress() const {
    return timeOfDayHours_ / 24.0;
}

bool WorldClock::IsNight() const {
    return timeOfDayHours_ < 6.0 || timeOfDayHours_ >= 20.0;
}
