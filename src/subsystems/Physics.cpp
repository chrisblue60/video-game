#include "subsystems/Physics.hpp"

#include <iostream>

const char* Physics::Name() const { return "Physics"; }

bool Physics::Init(const SubsystemContext& context) {
    verboseLogging_ = context.config.verboseLogging;
    std::cout << "[Physics] Init (solver pipeline stub ready)\n";

    player_ = world_.CreateEntity();
    world_.SetTransform(player_, Transform{{0.0F, 10.0F, 0.0F}, {}, {1.0F, 1.0F, 1.0F}});
    world_.SetVelocity(player_, Velocity{{1.5F, 0.0F, 0.0F}});
    world_.SetGravity(player_, Gravity{-9.81F});
    world_.SetCollider(player_, Collider{0.9F, 0.35F});
    return true;
}

void Physics::Tick(const FrameContext& frame) {
    const float dt = static_cast<float>(frame.fixedDeltaSeconds);
    for (Entity entity : world_.Entities()) {
        const auto tr = world_.GetTransform(entity);
        const auto vel = world_.GetVelocity(entity);
        if (!tr.has_value() || !vel.has_value()) {
            continue;
        }

        Velocity nextVel = *vel;
        if (const auto gravity = world_.GetGravity(entity); gravity.has_value()) {
            nextVel.linear.y += gravity->accelerationY * dt;
            world_.SetVelocity(entity, nextVel);
        }

        Transform next = *tr;
        next.position.x += nextVel.linear.x * dt;
        next.position.y += nextVel.linear.y * dt;
        next.position.z += nextVel.linear.z * dt;

        float groundContactY = 0.0F;
        if (const auto collider = world_.GetCollider(entity); collider.has_value()) {
            groundContactY = collider->halfHeight;
        }

        if (next.position.y < groundContactY) {
            next.position.y = groundContactY;
            nextVel.linear.y = 0.0F;
            world_.SetVelocity(entity, nextVel);
        }

        world_.SetTransform(entity, next);
    }

    if (!verboseLogging_ && (frame.frameIndex % 120 != 0)) {
        return;
    }

    const auto playerTransform = world_.GetTransform(player_);
    std::cout << "[Physics] Frame=" << frame.frameIndex << " dt=" << frame.fixedDeltaSeconds;
    if (playerTransform.has_value()) {
        std::cout << " playerX=" << playerTransform->position.x << " playerY=" << playerTransform->position.y;
    }
    std::cout << "\n";
}

void Physics::Shutdown() { std::cout << "[Physics] Shutdown\n"; }
