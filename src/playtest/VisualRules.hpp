#pragma once

#include "playtest/PlaytestState.hpp"

struct ScreenPoint {
    int x = 0;
    int y = 0;
    bool visible = false;
};

struct VisualScaleRules {
    float focalLength = 550.0F;
    float minDepth = 0.1F;
    float minScale = 0.35F;
    float maxScale = 2.8F;
    float referenceDepth = 14.0F;
    float depthBias = 0.5F;
};

class VisualRules {
public:
    static float CameraDepth(const PlaytestState& state, float worldX, float worldZ);
    static ScreenPoint ProjectPoint(const PlaytestState& state, int screenWidth, int screenHeight, float worldX, float worldY,
                                    float worldZ, const VisualScaleRules& rules = {});
    static float PerspectiveScale(float depth, const VisualScaleRules& rules = {});
};
