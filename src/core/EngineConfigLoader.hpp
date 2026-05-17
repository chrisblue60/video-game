#pragma once

#include <string>

#include "core/EngineConfig.hpp"

class EngineConfigLoader {
public:
    static EngineConfig LoadFromFileOrDefault(const std::string& path);
};
