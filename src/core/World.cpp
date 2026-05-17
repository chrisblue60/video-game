#include "core/World.hpp"

Entity World::CreateEntity() { return entities_.Create(); }

bool World::DestroyEntity(Entity entity) {
    transforms_.Remove(entity);
    velocities_.Remove(entity);
    gravities_.Remove(entity);
    colliders_.Remove(entity);
    return entities_.Destroy(entity);
}

void World::SetTransform(Entity entity, const Transform& transform) {
    if (!entities_.IsAlive(entity)) return;
    transforms_.Set(entity, transform);
}

bool World::RemoveTransform(Entity entity) { return transforms_.Remove(entity); }
std::optional<Transform> World::GetTransform(Entity entity) const { return transforms_.Get(entity); }

void World::SetVelocity(Entity entity, const Velocity& velocity) {
    if (!entities_.IsAlive(entity)) return;
    velocities_.Set(entity, velocity);
}

bool World::RemoveVelocity(Entity entity) { return velocities_.Remove(entity); }
std::optional<Velocity> World::GetVelocity(Entity entity) const { return velocities_.Get(entity); }

void World::SetGravity(Entity entity, const Gravity& gravity) {
    if (!entities_.IsAlive(entity)) return;
    gravities_.Set(entity, gravity);
}

bool World::RemoveGravity(Entity entity) { return gravities_.Remove(entity); }
std::optional<Gravity> World::GetGravity(Entity entity) const { return gravities_.Get(entity); }

void World::SetCollider(Entity entity, const Collider& collider) {
    if (!entities_.IsAlive(entity)) return;
    colliders_.Set(entity, collider);
}

bool World::RemoveCollider(Entity entity) { return colliders_.Remove(entity); }
std::optional<Collider> World::GetCollider(Entity entity) const { return colliders_.Get(entity); }

std::vector<Entity> World::Entities() const { return entities_.AliveEntities(); }

std::size_t World::EntityCount() const { return entities_.AliveCount(); }
std::size_t World::TransformCount() const { return transforms_.Count(); }
std::size_t World::VelocityCount() const { return velocities_.Count(); }
std::size_t World::GravityCount() const { return gravities_.Count(); }
std::size_t World::ColliderCount() const { return colliders_.Count(); }
