#pragma once

#include <optional>
#include <unordered_map>

#include "core/Entity.hpp"
#include "core/Gravity.hpp"

class GravityStore {
public:
    void Set(Entity entity, const Gravity& gravity);
    bool Remove(Entity entity);
    std::optional<Gravity> Get(Entity entity) const;
    std::size_t Count() const;

private:
    std::unordered_map<std::uint32_t, Gravity> gravities_;
};
