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

constexpr double PI = 3.14159265358979323846;

float DegToRad(float deg) { return deg * static_cast<float>(PI / 180.0); }

void SeedTargets(PlaytestState& state) {
    state.targets.clear();
    state.targets.push_back(TargetState{8.0F, 1.0F, 0.0F, true});
    state.targets.push_back(TargetState{14.0F, 1.0F, 3.5F, true});
    state.targets.push_back(TargetState{20.0F, 1.0F, -4.0F, true});
}

void EnsureTargetsSeeded(PlaytestState& state) {
    if (state.targets.empty()) SeedTargets(state);
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
    state.combat.roundTimeSeconds = 0.0F;
}

bool FireWeapon(PlaytestState& state) {
    if (state.weapon.fireCooldownSeconds > 0.0F || state.weapon.ammoInMag <= 0) return false;
    state.weapon.ammoInMag -= 1;
    state.camera.recoilPitchDeg += kRecoilKickDeg;
    state.weapon.shotsFired += 1;
    state.weapon.fireCooldownSeconds = kFireIntervalSeconds;
    return true;
}

void ResolveHitscan(PlaytestState& state) {
    const float yawRad = DegToRad(state.camera.yawDeg);
    const float pitchRad = DegToRad(state.camera.pitchDeg - state.camera.recoilPitchDeg);
    const float dirX = std::cos(yawRad) * std::cos(pitchRad);
    const float dirY = -std::sin(pitchRad);
    const float dirZ = std::sin(yawRad) * std::cos(pitchRad);

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
            return;
        }
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
    if (input.fire && FireWeapon(state)) ResolveHitscan(state);
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
    if (input.resetRound) ResetRound(state);

    for (TargetState& target : state.targets) {
        target.hitFlashSeconds = std::max(0.0F, target.hitFlashSeconds - dt);
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
    const float forwardX = std::cos(yawRad), forwardZ = std::sin(yawRad);
    const float rightX = -forwardZ, rightZ = forwardX;
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

    state.camera.x = state.player.x;
    state.camera.y = state.player.y + 0.8F;
    state.camera.z = state.player.z;

    UpdateWeapon(state, input, dt);
    UpdateRoundProgress(state, dt);
    state.clock.Advance(dt, kWorldHoursPerSecond);
}
