#include "core/VelocityStore.hpp"

void VelocityStore::Set(Entity entity, const Velocity& velocity) {
    velocities_[entity.id] = velocity;
}

bool VelocityStore::Remove(Entity entity) {
    return velocities_.erase(entity.id) > 0;
}

std::optional<Velocity> VelocityStore::Get(Entity entity) const {
    const auto it = velocities_.find(entity.id);
    if (it == velocities_.end()) {
        return std::nullopt;
    }

    return it->second;
}

std::size_t VelocityStore::Count() const {
    return velocities_.size();
}
