#pragma once

#include <functional>

#include "core/EngineConfig.hpp"
#include "core/FrameContext.hpp"

class GameLoop {
public:
    using TickFn = std::function<void(const FrameContext&)>;
    using RenderFn = std::function<void(const FrameContext&)>;

    void Run(const EngineConfig& config, const TickFn& tick, const RenderFn& render);
};
