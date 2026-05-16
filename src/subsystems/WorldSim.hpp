#pragma once

#include "core/ISubsystem.hpp"
#include "core/WorldClock.hpp"

class WorldSim final : public ISubsystem {
public:
    const char* Name() const override;
    bool Init(const SubsystemContext& context) override;
    void Tick(const FrameContext& frame) override;
    void Shutdown() override;

private:
    WorldClock clock_;
    double worldHoursPerRealSecond_ = 0.25;
    bool verboseLogging_ = false;
};
