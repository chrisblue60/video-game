#include "core/EntityManager.hpp"

Entity EntityManager::Create() {
    Entity entity{nextId_++};
    alive_.insert(entity.id);
    return entity;
}

bool EntityManager::Destroy(Entity entity) {
    return alive_.erase(entity.id) > 0;
}

bool EntityManager::IsAlive(Entity entity) const {
    return alive_.contains(entity.id);
}

std::size_t EntityManager::AliveCount() const {
    return alive_.size();
}

std::vector<Entity> EntityManager::AliveEntities() const {
    std::vector<Entity> result;
    result.reserve(alive_.size());
    for (std::uint32_t id : alive_) {
        result.push_back(Entity{id});
    }
    return result;
}
