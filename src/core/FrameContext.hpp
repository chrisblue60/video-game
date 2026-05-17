#pragma once

struct FrameContext {
    int frameIndex = 0;
    double fixedDeltaSeconds = 0.0;
    double interpolationAlpha = 0.0;
};
