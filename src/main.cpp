#include "core/Engine.hpp"
#include "core/EngineConfigLoader.hpp"

#include <cstdlib>
#include <iostream>

int main() {
    EngineConfig config = EngineConfigLoader::LoadFromFileOrDefault("config/engine.cfg");

    Engine engine(config);

    if (!engine.Init()) {
        std::cerr << "Engine initialization failed\n";
        return EXIT_FAILURE;
    }

    engine.Run();
    engine.Shutdown();
    return EXIT_SUCCESS;
}
