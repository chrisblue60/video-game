#include "core/SubsystemPipeline.hpp"

#include <iostream>

void SubsystemPipeline::Register(ISubsystem* subsystem) {
    subsystems_.push_back(subsystem);
}

bool SubsystemPipeline::InitAll(const SubsystemContext& context) {
    for (ISubsystem* subsystem : subsystems_) {
        if (!subsystem->Init(context)) {
            std::cerr << "[Engine] Failed to initialize subsystem: " << subsystem->Name() << "\n";
            return false;
        }
    }

    return true;
}

void SubsystemPipeline::TickAll(const FrameContext& frame) {
    for (ISubsystem* subsystem : subsystems_) {
        subsystem->Tick(frame);
    }
}

void SubsystemPipeline::ShutdownAll() {
    for (auto it = subsystems_.rbegin(); it != subsystems_.rend(); ++it) {
        (*it)->Shutdown();
    }
}
