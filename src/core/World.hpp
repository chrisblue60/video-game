#pragma once

#include <vector>

#include "core/ColliderStore.hpp"
#include "core/EntityManager.hpp"
#include "core/GravityStore.hpp"
#include "core/TransformStore.hpp"
#include "core/VelocityStore.hpp"

class World {
public:
    Entity CreateEntity();
    bool DestroyEntity(Entity entity);

    void SetTransform(Entity entity, const Transform& transform);
    bool RemoveTransform(Entity entity);
    std::optional<Transform> GetTransform(Entity entity) const;

    void SetVelocity(Entity entity, const Velocity& velocity);
    bool RemoveVelocity(Entity entity);
    std::optional<Velocity> GetVelocity(Entity entity) const;

    void SetGravity(Entity entity, const Gravity& gravity);
    bool RemoveGravity(Entity entity);
    std::optional<Gravity> GetGravity(Entity entity) const;

    void SetCollider(Entity entity, const Collider& collider);
    bool RemoveCollider(Entity entity);
    std::optional<Collider> GetCollider(Entity entity) const;

    std::vector<Entity> Entities() const;

    std::size_t EntityCount() const;
    std::size_t TransformCount() const;
    std::size_t VelocityCount() const;
    std::size_t GravityCount() const;
    std::size_t ColliderCount() const;

private:
    EntityManager entities_;
    TransformStore transforms_;
    VelocityStore velocities_;
    GravityStore gravities_;
    ColliderStore colliders_;
};
