#include "subsystems/WorldSim.hpp"

#include <iostream>

const char* WorldSim::Name() const {
    return "WorldSim";
}

bool WorldSim::Init(const SubsystemContext& context) {
    worldHoursPerRealSecond_ = context.config.worldHoursPerRealSecond;
    verboseLogging_ = context.config.verboseLogging;
    clock_.SetTimeOfDayHours(8.0);
    std::cout << "[WorldSim] Init (time/weather/economy simulation stub ready)\n";
    return true;
}

void WorldSim::Tick(const FrameContext& frame) {
    clock_.Advance(frame.fixedDeltaSeconds, worldHoursPerRealSecond_);

    if (!verboseLogging_ && (frame.frameIndex % 120 != 0)) {
        return;
    }

    std::cout << "[WorldSim] Frame=" << frame.frameIndex
              << " timeOfDay=" << clock_.TimeOfDayHours() << "h"
              << " dayProgress=" << clock_.NormalizedDayProgress()
              << " isNight=" << (clock_.IsNight() ? "yes" : "no") << "\n";
}

void WorldSim::Shutdown() {
    std::cout << "[WorldSim] Shutdown\n";
}
