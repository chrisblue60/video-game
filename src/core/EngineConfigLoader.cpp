#include "core/EngineConfigLoader.hpp"

#include <fstream>
#include <sstream>
#include <string>

namespace {

bool ParseBool(const std::string& value) {
    return value == "1" || value == "true" || value == "TRUE";
}

}  // namespace

EngineConfig EngineConfigLoader::LoadFromFileOrDefault(const std::string& path) {
    EngineConfig config{};

    std::ifstream input(path);
    if (!input.is_open()) {
        return config;
    }

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        const auto split = line.find('=');
        if (split == std::string::npos) {
            continue;
        }

        const std::string key = line.substr(0, split);
        const std::string value = line.substr(split + 1);

        std::istringstream parser(value);
        if (key == "fixedDeltaSeconds") {
            parser >> config.fixedDeltaSeconds;
        } else if (key == "maxFrameTimeSeconds") {
            parser >> config.maxFrameTimeSeconds;
        } else if (key == "maxFrames") {
            parser >> config.maxFrames;
        } else if (key == "worldHoursPerRealSecond") {
            parser >> config.worldHoursPerRealSecond;
        } else if (key == "targetFrameMillis") {
            parser >> config.targetFrameMillis;
        } else if (key == "verboseLogging") {
            config.verboseLogging = ParseBool(value);
        }
    }

    return config;
}
