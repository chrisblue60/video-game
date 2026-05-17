#pragma once

#include <vector>

#include "core/ISubsystem.hpp"

class SubsystemPipeline {
public:
    void Register(ISubsystem* subsystem);
    bool InitAll(const SubsystemContext& context);
    void TickAll(const FrameContext& frame);
    void ShutdownAll();

private:
    std::vector<ISubsystem*> subsystems_;
};
