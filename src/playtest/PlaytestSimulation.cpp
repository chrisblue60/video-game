#include "playtest/PlaytestSimulation.hpp"

#include <algorithm>
#include <cmath>

namespace {
constexpr float kWalkSpeed = 5.0F;
constexpr float kSprintSpeed = 8.5F;
constexpr float kMouseSensitivity = 0.12F;
constexpr float kGravity = -20.0F;
constexpr float kJumpVelocity = 7.5F;
constexpr float kGroundY = 0.9F;
constexpr float kWorldHoursPerSecond = 0.35F;
constexpr float kFireIntervalSeconds = 0.10F;
constexpr int kMagSize = 30;
constexpr float kHitDistance = 30.0F;
constexpr float kHitCosThreshold = 0.98F;
constexpr float kHitFlashSeconds = 0.15F;
constexpr float kRecoilKickDeg = 1.3F;
constexpr float kRecoilRecoverDegPerSec = 7.0F;
constexpr float kObjectHitFlashSeconds = 0.18F;
constexpr float kPlayerCollisionRadius = 0.45F;
constexpr float kWorldBounds = 24.0F;

constexpr double PI = 3.14159265358979323846;

float DegToRad(float deg) { return deg * static_cast<float>(PI / 180.0); }

void SeedTargets(PlaytestState& state) {
    state.targets.clear();
    state.targets.push_back(TargetState{0.0F, 1.0F, 8.0F, true});
    state.targets.push_back(TargetState{3.5F, 1.0F, 14.0F, true});
    state.targets.push_back(TargetState{-4.0F, 1.0F, 20.0F, true});
}

void EnsureTargetsSeeded(PlaytestState& state) {
    if (state.targets.empty()) SeedTargets(state);
}

void EnsureWorldSeeded(PlaytestState& state) {
    if (!state.worldObjects.empty()) return;
    state.worldObjects.push_back(WorldObject{"Library Terminal", -6.0F, 0.9F, 12.0F, 2.2F, 1.0F, InteractionType::LibraryTerminal});
    state.worldObjects.push_back(WorldObject{"Build Parcel A", 6.0F, 0.9F, 10.0F, 4.0F, 1.2F, InteractionType::BuildParcel});
}

int CountAliveTargets(const PlaytestState& state) {
    int alive = 0;
    for (const auto& t : state.targets) alive += t.alive ? 1 : 0;
    return alive;
}

void ResetRound(PlaytestState& state) {
    SeedTargets(state);
    state.weapon.ammoInMag = kMagSize;
    state.weapon.reserveAmmo = 90;
    state.weapon.fireCooldownSeconds = 0.0F;
    state.combat.targetsHit = 0;
    state.weapon.shotsFired = 0;
    state.weapon.shotsHit = 0;
    state.combat.roundTimeSeconds = 0.0F;
}

float DistanceSq(float ax, float ay, float az, float bx, float by, float bz) {
    const float dx = ax - bx;
    const float dy = ay - by;
    const float dz = az - bz;
    return dx * dx + dy * dy + dz * dz;
}

void UpdateWorldRules(PlaytestState& state, const PlaytestInput& input) {
    state.worldRules.inBuildParcel = false;
    state.worldRules.canPlaceNow = false;
    for (const auto& object : state.worldObjects) {
        if (object.interaction != InteractionType::BuildParcel) continue;
        const float d2 = DistanceSq(state.player.x, state.player.y, state.player.z, object.x, object.y, object.z);
        if (d2 <= object.interactionRadius * object.interactionRadius) {
            state.worldRules.inBuildParcel = true;
            break;
        }
    }

    state.worldRules.canPlaceNow = state.worldRules.inBuildParcel && state.worldRules.buildCredits > 0;

    if (input.interact) {
        for (const auto& object : state.worldObjects) {
            const float d2 = DistanceSq(state.player.x, state.player.y, state.player.z, object.x, object.y, object.z);
            if (d2 > object.interactionRadius * object.interactionRadius) continue;
            if (object.interaction == InteractionType::LibraryTerminal) {
                state.worldRules.lastInteraction = "Library opened: minigame and world-building guides available.";
                return;
            }
            if (object.interaction == InteractionType::BuildParcel) {
                state.worldRules.lastInteraction = "Parcel rules: place structures with B while credits remain.";
                return;
            }
        }
    }

    if (input.placeStructure) {
        if (state.worldRules.canPlaceNow) {
            state.worldRules.buildCredits -= 1;
            state.worldRules.placedStructures += 1;
            state.worldRules.lastInteraction = "Structure placed in parcel.";
        } else if (!state.worldRules.inBuildParcel) {
            state.worldRules.lastInteraction = "Cannot place: move into a build parcel.";
        } else {
            state.worldRules.lastInteraction = "Cannot place: no build credits.";
        }
    }
}

bool FireWeapon(PlaytestState& state) {
    if (state.weapon.fireCooldownSeconds > 0.0F || state.weapon.ammoInMag <= 0) return false;
    state.weapon.ammoInMag -= 1;
    state.weapon.shotsFired += 1;
    state.weapon.fireCooldownSeconds = kFireIntervalSeconds;
    return true;
}

void ResolveHitscan(PlaytestState& state) {
    const float yawRad = DegToRad(state.camera.yawDeg);
    const float pitchRad = DegToRad(state.camera.pitchDeg + state.camera.recoilPitchDeg);
    const float dirX = std::sin(yawRad) * std::cos(pitchRad);
    const float dirY = -std::sin(pitchRad);
    const float dirZ = std::cos(yawRad) * std::cos(pitchRad);

    for (TargetState& target : state.targets) {
        if (!target.alive) continue;
        const float dx = target.x - state.camera.x;
        const float dy = target.y - state.camera.y;
        const float dz = target.z - state.camera.z;
        const float distSq = dx * dx + dy * dy + dz * dz;
        if (distSq > kHitDistance * kHitDistance) continue;

        const float invDist = 1.0F / std::sqrt(distSq);
        const float toTargetX = dx * invDist;
        const float toTargetY = dy * invDist;
        const float toTargetZ = dz * invDist;
        const float dot = dirX * toTargetX + dirY * toTargetY + dirZ * toTargetZ;
        if (dot >= kHitCosThreshold) {
            target.alive = false;
            target.hitFlashSeconds = kHitFlashSeconds;
            state.combat.targetsHit += 1;
            state.weapon.shotsHit += 1;
            return;
        }
    }

    for (WorldObject& object : state.worldObjects) {
        const float dx = object.x - state.camera.x;
        const float dy = object.y - state.camera.y;
        const float dz = object.z - state.camera.z;
        const float distSq = dx * dx + dy * dy + dz * dz;
        if (distSq > kHitDistance * kHitDistance) continue;
        const float invDist = 1.0F / std::sqrt(distSq);
        const float dot = dirX * (dx * invDist) + dirY * (dy * invDist) + dirZ * (dz * invDist);
        if (dot >= kHitCosThreshold) {
            object.hitFlashSeconds = kObjectHitFlashSeconds;
            state.worldRules.lastInteraction = "Impact: world object is solid (not a combat target).";
            return;
        }
    }
}

void ResolvePlayerCollisions(PlaytestState& state) {
    state.player.x = std::clamp(state.player.x, -kWorldBounds, kWorldBounds);
    state.player.z = std::clamp(state.player.z, -kWorldBounds, kWorldBounds);

    auto pushOut = [&](float ox, float oz, float obstacleRadius) {
        const float dx = state.player.x - ox;
        const float dz = state.player.z - oz;
        const float minDist = kPlayerCollisionRadius + obstacleRadius;
        const float distSq = dx * dx + dz * dz;
        if (distSq >= minDist * minDist) return;
        if (distSq < 0.000001F) {
            state.player.x = ox + minDist;
            return;
        }
        const float dist = std::sqrt(distSq);
        const float nx = dx / dist;
        const float nz = dz / dist;
        state.player.x = ox + nx * minDist;
        state.player.z = oz + nz * minDist;
    };

    for (const auto& object : state.worldObjects) {
        pushOut(object.x, object.z, object.collisionRadius);
    }
    for (const auto& target : state.targets) {
        if (!target.alive) continue;
        pushOut(target.x, target.z, 0.45F);
    }
}

void UpdateWeapon(PlaytestState& state, const PlaytestInput& input, float dt) {
    state.weapon.fireCooldownSeconds = std::max(0.0F, state.weapon.fireCooldownSeconds - dt);
    if (input.reload && state.weapon.ammoInMag < kMagSize && state.weapon.reserveAmmo > 0) {
        const int need = kMagSize - state.weapon.ammoInMag;
        const int load = std::min(need, state.weapon.reserveAmmo);
        state.weapon.ammoInMag += load;
        state.weapon.reserveAmmo -= load;
    }
    if (input.fire && FireWeapon(state)) {
        ResolveHitscan(state);
        state.camera.recoilPitchDeg += kRecoilKickDeg;
    }
}

void UpdateRoundProgress(PlaytestState& state, float dt) {
    state.combat.roundTimeSeconds += dt;
    if (CountAliveTargets(state) == 0) {
        state.combat.roundsCompleted += 1;
        if (state.combat.bestRoundSeconds <= 0.0F || state.combat.roundTimeSeconds < state.combat.bestRoundSeconds) {
            state.combat.bestRoundSeconds = state.combat.roundTimeSeconds;
        }
        ResetRound(state);
    }
}

}  // namespace

void PlaytestSimulation::Step(PlaytestState& state, const PlaytestInput& input, float dt) {
    EnsureTargetsSeeded(state);
    EnsureWorldSeeded(state);
    if (input.resetRound) ResetRound(state);

    for (TargetState& target : state.targets) {
        target.hitFlashSeconds = std::max(0.0F, target.hitFlashSeconds - dt);
    }
    for (WorldObject& object : state.worldObjects) {
        object.hitFlashSeconds = std::max(0.0F, object.hitFlashSeconds - dt);
    }

    state.camera.yawDeg += input.mouseDeltaX * kMouseSensitivity;
    state.camera.pitchDeg -= input.mouseDeltaY * kMouseSensitivity;
    state.camera.recoilPitchDeg = std::max(0.0F, state.camera.recoilPitchDeg - (kRecoilRecoverDegPerSec * dt));
    state.camera.pitchDeg = std::clamp(state.camera.pitchDeg, -89.0F, 89.0F);

    float moveForward = 0.0F, moveRight = 0.0F;
    if (input.moveForward) moveForward += 1.0F;
    if (input.moveBackward) moveForward -= 1.0F;
    if (input.moveRight) moveRight += 1.0F;
    if (input.moveLeft) moveRight -= 1.0F;

    const float speed = input.sprint ? kSprintSpeed : kWalkSpeed;
    const float yawRad = DegToRad(state.camera.yawDeg);
    const float forwardX = std::sin(yawRad), forwardZ = std::cos(yawRad);
    const float rightX = std::cos(yawRad), rightZ = -std::sin(yawRad);
    state.player.vx = (forwardX * moveForward + rightX * moveRight) * speed;
    state.player.vz = (forwardZ * moveForward + rightZ * moveRight) * speed;

    const bool onGround = state.player.y <= kGroundY + 0.0001F;
    if (input.jump && onGround) state.player.vy = kJumpVelocity;
    else state.player.vy += kGravity * dt;

    state.player.x += state.player.vx * dt;
    state.player.y += state.player.vy * dt;
    state.player.z += state.player.vz * dt;
    if (state.player.y < kGroundY) {
        state.player.y = kGroundY;
        state.player.vy = 0.0F;
    }
    ResolvePlayerCollisions(state);

    state.camera.x = state.player.x;
    state.camera.y = state.player.y + 0.8F;
    state.camera.z = state.player.z;

    UpdateWeapon(state, input, dt);
    UpdateRoundProgress(state, dt);
    UpdateWorldRules(state, input);
    state.clock.Advance(dt, kWorldHoursPerSecond);
}
