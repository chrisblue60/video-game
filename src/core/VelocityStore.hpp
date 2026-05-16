#pragma once

#include <optional>
#include <unordered_map>

#include "core/Entity.hpp"
#include "core/Velocity.hpp"

class VelocityStore {
public:
    void Set(Entity entity, const Velocity& velocity);
    bool Remove(Entity entity);
    std::optional<Velocity> Get(Entity entity) const;
    std::size_t Count() const;

private:
    std::unordered_map<std::uint32_t, Velocity> velocities_;
};
