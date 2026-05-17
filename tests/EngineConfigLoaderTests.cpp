#include "core/EngineConfigLoader.hpp"

#include <fstream>
#include <iostream>
#include <string>

namespace {

int Fail(const char* message) {
    std::cerr << "[FAIL] " << message << "\n";
    return 1;
}

bool NearlyEqual(double a, double b, double epsilon = 1e-6) {
    return (a > b ? a - b : b - a) < epsilon;
}

}  // namespace

int main() {
    const std::string testPath = "./engine_test.cfg";

    {
        std::ofstream out(testPath);
        out << "fixedDeltaSeconds=0.02\n";
        out << "maxFrameTimeSeconds=0.3\n";
        out << "maxFrames=120\n";
        out << "worldHoursPerRealSecond=0.5\n";
        out << "targetFrameMillis=10\n";
        out << "verboseLogging=true\n";
    }

    EngineConfig loaded = EngineConfigLoader::LoadFromFileOrDefault(testPath);

    if (!NearlyEqual(loaded.fixedDeltaSeconds, 0.02)) {
        return Fail("fixedDeltaSeconds was not parsed");
    }
    if (!NearlyEqual(loaded.maxFrameTimeSeconds, 0.3)) {
        return Fail("maxFrameTimeSeconds was not parsed");
    }
    if (loaded.maxFrames != 120) {
        return Fail("maxFrames was not parsed");
    }
    if (!NearlyEqual(loaded.worldHoursPerRealSecond, 0.5)) {
        return Fail("worldHoursPerRealSecond was not parsed");
    }
    if (loaded.targetFrameMillis != 10) {
        return Fail("targetFrameMillis was not parsed");
    }
    if (!loaded.verboseLogging) {
        return Fail("verboseLogging was not parsed");
    }

    EngineConfig defaults = EngineConfigLoader::LoadFromFileOrDefault("./non-existent-config.cfg");
    if (defaults.maxFrames != 600) {
        return Fail("defaults should be returned for missing file");
    }

    std::remove(testPath.c_str());
    std::cout << "[PASS] EngineConfigLoader tests\n";
    return 0;
}
