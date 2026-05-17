#pragma once

#include <cstdint>

struct Entity {
    std::uint32_t id = 0;

    bool operator==(const Entity& other) const { return id == other.id; }
    bool operator!=(const Entity& other) const { return !(*this == other); }
};
