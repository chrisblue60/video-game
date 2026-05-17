#pragma once

#include <unordered_set>
#include <vector>

#include "core/Entity.hpp"

class EntityManager {
public:
    Entity Create();
    bool Destroy(Entity entity);
    bool IsAlive(Entity entity) const;
    std::size_t AliveCount() const;
    std::vector<Entity> AliveEntities() const;

private:
    std::uint32_t nextId_ = 1;
    std::unordered_set<std::uint32_t> alive_;
};
