#include "core/GameLoop.hpp"

#include <algorithm>
#include <chrono>
#include <thread>

void GameLoop::Run(const EngineConfig& config, const TickFn& tick, const RenderFn& render) {
    using Clock = std::chrono::steady_clock;

    auto previous = Clock::now();
    double accumulator = 0.0;
    int frameIndex = 0;

    while (frameIndex < config.maxFrames) {
        auto now = Clock::now();
        std::chrono::duration<double> elapsed = now - previous;
        previous = now;

        const double frameTime = std::min(elapsed.count(), config.maxFrameTimeSeconds);
        accumulator += frameTime;

        while (accumulator >= config.fixedDeltaSeconds) {
            FrameContext tickContext{};
            tickContext.frameIndex = frameIndex;
            tickContext.fixedDeltaSeconds = config.fixedDeltaSeconds;
            tick(tickContext);
            accumulator -= config.fixedDeltaSeconds;
        }

        FrameContext renderContext{};
        renderContext.frameIndex = frameIndex;
        renderContext.fixedDeltaSeconds = config.fixedDeltaSeconds;
        renderContext.interpolationAlpha = accumulator / config.fixedDeltaSeconds;
        render(renderContext);

        ++frameIndex;
        std::this_thread::sleep_for(std::chrono::milliseconds(config.targetFrameMillis));
    }
}
