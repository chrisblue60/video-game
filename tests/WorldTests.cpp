#include "core/World.hpp"

#include <iostream>

namespace {
int Fail(const char* message) {
    std::cerr << "[FAIL] " << message << "\n";
    return 1;
}
}  // namespace

int main() {
    World world;
    Entity e1 = world.CreateEntity();
    Entity e2 = world.CreateEntity();
    if (e1.id == e2.id) return Fail("Entities must receive unique IDs");

    world.SetTransform(e1, Transform{});
    world.SetVelocity(e1, Velocity{{3.0F, 1.0F, 0.0F}});
    world.SetGravity(e1, Gravity{-9.81F});
    world.SetCollider(e1, Collider{0.5F, 0.3F});

    if (!world.GetTransform(e1).has_value()) return Fail("Transform should be present");
    if (!world.GetVelocity(e1).has_value()) return Fail("Velocity should be present");
    if (!world.GetGravity(e1).has_value()) return Fail("Gravity should be present");
    if (!world.GetCollider(e1).has_value()) return Fail("Collider should be present");

    if (world.ColliderCount() != 1) return Fail("ColliderCount should be 1");

    if (!world.DestroyEntity(e1)) return Fail("DestroyEntity should succeed");
    if (world.GetTransform(e1).has_value()) return Fail("DestroyEntity should remove transform");
    if (world.GetVelocity(e1).has_value()) return Fail("DestroyEntity should remove velocity");
    if (world.GetGravity(e1).has_value()) return Fail("DestroyEntity should remove gravity");
    if (world.GetCollider(e1).has_value()) return Fail("DestroyEntity should remove collider");

    std::cout << "[PASS] World tests\n";
    return 0;
}
