#pragma once

#include "playtest/PlaytestState.hpp"

enum class BulletHitKind {
    None,
    Target,
    WorldObject,
};

struct BulletHitResult {
    BulletHitKind kind = BulletHitKind::None;
    int index = -1;
    float distance = 0.0F;
};

class BulletRules {
public:
    static BulletHitResult ResolveFirstHit(const PlaytestState& state, float originX, float originY, float originZ, float dirX,
                                           float dirY, float dirZ, float maxDistance);
};
