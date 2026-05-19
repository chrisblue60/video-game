#pragma once

struct EngineConfig {
    double fixedDeltaSeconds = 1.0 / 60.0;
    double maxFrameTimeSeconds = 0.25;
    int maxFrames = 600;
    double worldHoursPerRealSecond = 0.25;
    int targetFrameMillis = 16;
    bool verboseLogging = false;
};
