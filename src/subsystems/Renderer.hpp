#pragma once

#include "core/ISubsystem.hpp"

class Renderer final : public ISubsystem {
public:
    const char* Name() const override;
    bool Init(const SubsystemContext& context) override;
    void Tick(const FrameContext& frame) override;
    void Shutdown() override;

private:
    bool verboseLogging_ = false;
};
