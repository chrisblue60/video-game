#include "core/ColliderStore.hpp"

void ColliderStore::Set(Entity entity, const Collider& collider) {
    colliders_[entity.id] = collider;
}

bool ColliderStore::Remove(Entity entity) {
    return colliders_.erase(entity.id) > 0;
}

std::optional<Collider> ColliderStore::Get(Entity entity) const {
    const auto it = colliders_.find(entity.id);
    if (it == colliders_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::size_t ColliderStore::Count() const {
    return colliders_.size();
}
