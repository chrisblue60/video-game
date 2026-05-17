#pragma once

#include "core/ISubsystem.hpp"
#include "core/World.hpp"

class Physics final : public ISubsystem {
public:
    const char* Name() const override;
    bool Init(const SubsystemContext& context) override;
    void Tick(const FrameContext& frame) override;
    void Shutdown() override;

private:
    World world_;
    Entity player_{};
    bool verboseLogging_ = false;
};
