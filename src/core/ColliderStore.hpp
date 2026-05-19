#pragma once

#include <optional>
#include <unordered_map>

#include "core/Collider.hpp"
#include "core/Entity.hpp"

class ColliderStore {
public:
    void Set(Entity entity, const Collider& collider);
    bool Remove(Entity entity);
    std::optional<Collider> Get(Entity entity) const;
    std::size_t Count() const;

private:
    std::unordered_map<std::uint32_t, Collider> colliders_;
};
