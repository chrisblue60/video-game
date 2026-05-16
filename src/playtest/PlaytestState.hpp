#pragma once

#include <vector>

#include "core/WorldClock.hpp"

struct CameraState {
    float x = 0.0F;
    float y = 1.7F;
    float z = 0.0F;
    float yawDeg = 0.0F;
    float pitchDeg = 0.0F;
};

struct PlayerState {
    float x = 0.0F;
    float y = 0.9F;
    float z = 0.0F;
    float vx = 0.0F;
    float vy = 0.0F;
    float vz = 0.0F;
};

struct WeaponState {
    int ammoInMag = 30;
    int reserveAmmo = 90;
    float fireCooldownSeconds = 0.0F;
    int shotsFired = 0;
};

struct TargetState {
    float x = 0.0F;
    float y = 1.0F;
    float z = 0.0F;
    bool alive = true;
};

struct CombatState {
    int targetsHit = 0;
    int roundsCompleted = 0;
    float roundTimeSeconds = 0.0F;
    float bestRoundSeconds = 0.0F;
};

struct PlaytestState {
    PlayerState player{};
    CameraState camera{};
    WeaponState weapon{};
    CombatState combat{};
    std::vector<TargetState> targets{};
    WorldClock clock{};
};
