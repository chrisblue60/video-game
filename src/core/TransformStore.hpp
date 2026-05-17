#pragma once

#include <optional>
#include <unordered_map>

#include "core/Entity.hpp"
#include "core/Transform.hpp"

class TransformStore {
public:
    void Set(Entity entity, const Transform& transform);
    bool Remove(Entity entity);
    std::optional<Transform> Get(Entity entity) const;
    std::size_t Count() const;

private:
    std::unordered_map<std::uint32_t, Transform> transforms_;
};
