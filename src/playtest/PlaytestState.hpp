#pragma once

#include <vector>

#include "core/WorldClock.hpp"
#include "playtest/WorldProperties.hpp"

struct CameraState {
    float x = 0.0F;
    float y = 1.7F;
    float z = 0.0F;
    float yawDeg = 0.0F;
    float pitchDeg = 0.0F;
    float recoilPitchDeg = 0.0F;
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
    int shotsHit = 0;
};

struct TargetState {
    float x = 0.0F;
    float y = 1.0F;
    float z = 0.0F;
    bool alive = true;
    float hitFlashSeconds = 0.0F;
};

struct CombatState {
    int targetsHit = 0;
    int roundsCompleted = 0;
    float roundTimeSeconds = 0.0F;
    float bestRoundSeconds = 0.0F;
};

enum class InteractionType {
    None,
    LibraryTerminal,
    BuildParcel,
};

struct WorldObject {
    const char* name = "";
    float x = 0.0F;
    float y = 0.0F;
    float z = 0.0F;
    float interactionRadius = 2.0F;
    float collisionRadius = 0.9F;
    InteractionType interaction = InteractionType::None;
    bool shootThrough = false; // Backward-compatible mirror of properties.combat.shootThrough.
    float hitFlashSeconds = 0.0F;
    WorldEntityProperties properties{};
};

struct WorldRulesState {
    int buildCredits = 4;
    int placedStructures = 0;
    bool inBuildParcel = false;
    bool canPlaceNow = false;
    const char* lastInteraction = "";
};

struct PlaytestState {
    PlayerState player{};
    CameraState camera{};
    WeaponState weapon{};
    CombatState combat{};
    std::vector<TargetState> targets{};
    std::vector<WorldObject> worldObjects{};
    WorldRulesState worldRules{};
    WorldClock clock{};
};
