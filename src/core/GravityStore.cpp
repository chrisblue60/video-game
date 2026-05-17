#include "core/GravityStore.hpp"

void GravityStore::Set(Entity entity, const Gravity& gravity) {
    gravities_[entity.id] = gravity;
}

bool GravityStore::Remove(Entity entity) {
    return gravities_.erase(entity.id) > 0;
}

std::optional<Gravity> GravityStore::Get(Entity entity) const {
    const auto it = gravities_.find(entity.id);
    if (it == gravities_.end()) {
        return std::nullopt;
    }
    return it->second;
}

std::size_t GravityStore::Count() const {
    return gravities_.size();
}
