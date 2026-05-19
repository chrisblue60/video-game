#include "playtest/BulletRules.hpp"

#include <cmath>

namespace {
bool RaySphereHit(float ox, float oy, float oz, float dx, float dy, float dz, float cx, float cy, float cz, float radius, float maxDist,
                  float& outDist) {
    const float lx = cx - ox;
    const float ly = cy - oy;
    const float lz = cz - oz;
    const float tca = lx * dx + ly * dy + lz * dz;
    if (tca < 0.0F) return false;
    const float d2 = (lx * lx + ly * ly + lz * lz) - (tca * tca);
    const float r2 = radius * radius;
    if (d2 > r2) return false;
    const float thc = std::sqrt(r2 - d2);
    const float t0 = tca - thc;
    const float t1 = tca + thc;
    const float t = t0 >= 0.0F ? t0 : t1;
    if (t < 0.0F || t > maxDist) return false;
    outDist = t;
    return true;
}
}  // namespace

BulletHitResult BulletRules::ResolveFirstHit(const PlaytestState& state, float originX, float originY, float originZ, float dirX, float dirY,
                                             float dirZ, float maxDistance) {
    BulletHitResult best{};
    best.distance = maxDistance + 1.0F;

    for (int i = 0; i < static_cast<int>(state.worldObjects.size()); ++i) {
        const auto& obj = state.worldObjects[static_cast<size_t>(i)];
        const float hitRadius = obj.properties.collision.radius > 0.0F ? obj.properties.collision.radius : obj.collisionRadius;
        float t = 0.0F;
        if (!RaySphereHit(originX, originY, originZ, dirX, dirY, dirZ, obj.x, obj.y + 0.8F, obj.z, hitRadius, maxDistance, t))
            continue;
        if (t < best.distance) {
            best = BulletHitResult{BulletHitKind::WorldObject, i, t};
        }
    }

    for (int i = 0; i < static_cast<int>(state.targets.size()); ++i) {
        const auto& target = state.targets[static_cast<size_t>(i)];
        if (!target.alive) continue;
        float t = 0.0F;
        if (!RaySphereHit(originX, originY, originZ, dirX, dirY, dirZ, target.x, target.y + 0.8F, target.z, 0.55F, maxDistance, t)) continue;
        if (t < best.distance) {
            best = BulletHitResult{BulletHitKind::Target, i, t};
        }
    }

    if (best.kind == BulletHitKind::WorldObject) {
        const auto& obj = state.worldObjects[static_cast<size_t>(best.index)];
        if (obj.shootThrough || obj.properties.combat.shootThrough) {
            // Try to find the nearest target that is behind this penetrable object.
            BulletHitResult afterObj{};
            afterObj.distance = maxDistance + 1.0F;
            for (int i = 0; i < static_cast<int>(state.targets.size()); ++i) {
                const auto& target = state.targets[static_cast<size_t>(i)];
                if (!target.alive) continue;
                float t = 0.0F;
                if (!RaySphereHit(originX, originY, originZ, dirX, dirY, dirZ, target.x, target.y + 0.8F, target.z, 0.55F, maxDistance, t))
                    continue;
                if (t > best.distance && t < afterObj.distance) afterObj = BulletHitResult{BulletHitKind::Target, i, t};
            }
            if (afterObj.kind != BulletHitKind::None) return afterObj;
        }
    }

    return best.kind == BulletHitKind::None ? BulletHitResult{} : best;
}
