#pragma once

struct Vec3 {
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;
};

struct Transform {
    Vec3 position{};
    Vec3 rotationEuler{};
    Vec3 scale{1.0F, 1.0F, 1.0F};
};
