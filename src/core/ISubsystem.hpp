#pragma once

#include "core/FrameContext.hpp"
#include "core/SubsystemContext.hpp"

class ISubsystem {
public:
    virtual ~ISubsystem() = default;

    virtual const char* Name() const = 0;
    virtual bool Init(const SubsystemContext& context) = 0;
    virtual void Tick(const FrameContext& frame) = 0;
    virtual void Shutdown() = 0;
};
