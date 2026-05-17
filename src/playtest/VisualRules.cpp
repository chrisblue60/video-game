#include "playtest/VisualRules.hpp"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kPi = 3.14159265358979323846F;
}

float VisualRules::CameraDepth(const PlaytestState& state, float worldX, float worldZ) {
    const float yaw = state.camera.yawDeg * kPi / 180.0F;
    const float dx = worldX - state.camera.x;
    const float dz = worldZ - state.camera.z;
    return std::sin(yaw) * dx + std::cos(yaw) * dz;
}

ScreenPoint VisualRules::ProjectPoint(const PlaytestState& state, int screenWidth, int screenHeight, float worldX, float worldY,
                                      float worldZ, const VisualScaleRules& rules) {
    const float yaw = state.camera.yawDeg * kPi / 180.0F;
    const float pitch = (state.camera.pitchDeg + state.camera.recoilPitchDeg) * kPi / 180.0F;
    const float dx = worldX - state.camera.x;
    const float dy = worldY - state.camera.y;
    const float dz = worldZ - state.camera.z;

    const float cy = std::cos(yaw);
    const float sy = std::sin(yaw);
    const float x1 = cy * dx - sy * dz;
    const float z1 = sy * dx + cy * dz;
    const float y1 = dy;

    const float cp = std::cos(pitch);
    const float sp = std::sin(pitch);
    const float y2 = cp * y1 - sp * z1;
    const float z2 = sp * y1 + cp * z1;
    if (z2 <= rules.minDepth) return {};

    const int sx = static_cast<int>(screenWidth * 0.5F + (x1 / z2) * rules.focalLength);
    const int sy2 = static_cast<int>(screenHeight * 0.5F - (y2 / z2) * rules.focalLength);
    const bool visible = sx >= -50 && sx <= screenWidth + 50 && sy2 >= -50 && sy2 <= screenHeight + 50;
    return ScreenPoint{sx, sy2, visible};
}

float VisualRules::PerspectiveScale(float depth, const VisualScaleRules& rules) {
    if (depth <= rules.minDepth) return 0.0F;
    return std::clamp(rules.referenceDepth / (depth + rules.depthBias), rules.minScale, rules.maxScale);
}
