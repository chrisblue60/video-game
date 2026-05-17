#include "core/TransformStore.hpp"

void TransformStore::Set(Entity entity, const Transform& transform) {
    transforms_[entity.id] = transform;
}

bool TransformStore::Remove(Entity entity) {
    return transforms_.erase(entity.id) > 0;
}

std::optional<Transform> TransformStore::Get(Entity entity) const {
    auto it = transforms_.find(entity.id);
    if (it == transforms_.end()) {
        return std::nullopt;
    }

    return it->second;
}

std::size_t TransformStore::Count() const {
    return transforms_.size();
}
