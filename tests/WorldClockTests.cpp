#include "core/WorldClock.hpp"

#include <cmath>
#include <iostream>

namespace {

bool NearlyEqual(double a, double b, double epsilon = 1e-6) {
    return std::fabs(a - b) < epsilon;
}

int Fail(const char* message) {
    std::cerr << "[FAIL] " << message << "\n";
    return 1;
}

}  // namespace

int main() {
    WorldClock clock;

    clock.SetTimeOfDayHours(25.5);
    if (!NearlyEqual(clock.TimeOfDayHours(), 1.5)) {
        return Fail("SetTimeOfDayHours should wrap beyond 24h");
    }

    clock.SetTimeOfDayHours(-3.0);
    if (!NearlyEqual(clock.TimeOfDayHours(), 21.0)) {
        return Fail("SetTimeOfDayHours should wrap negative values");
    }

    clock.SetTimeOfDayHours(19.9);
    if (clock.IsNight()) {
        return Fail("19.9 should still be day");
    }

    clock.SetTimeOfDayHours(20.0);
    if (!clock.IsNight()) {
        return Fail("20.0 should be night");
    }

    clock.SetTimeOfDayHours(5.99);
    if (!clock.IsNight()) {
        return Fail("5.99 should be night");
    }

    clock.SetTimeOfDayHours(6.0);
    if (clock.IsNight()) {
        return Fail("6.0 should be day");
    }

    clock.SetTimeOfDayHours(12.0);
    if (!NearlyEqual(clock.NormalizedDayProgress(), 0.5)) {
        return Fail("Normalized day progress at noon should be 0.5");
    }

    clock.SetTimeOfDayHours(23.5);
    clock.Advance(2.0, 0.5);
    if (!NearlyEqual(clock.TimeOfDayHours(), 0.5)) {
        return Fail("Advance should wrap correctly past midnight");
    }

    std::cout << "[PASS] WorldClock tests\n";
    return 0;
}
