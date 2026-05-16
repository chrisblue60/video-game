#include "core/Engine.hpp"

#include <iostream>

#include "core/SubsystemContext.hpp"

Engine::Engine(EngineConfig config)
    : config_(config) {
    simulationPipeline_.Register(&physics_);
    simulationPipeline_.Register(&worldSim_);
}

bool Engine::Init() {
    std::cout << "[Engine] Init\n";

    const SubsystemContext context{config_};
    if (!renderer_.Init(context)) {
        return false;
    }

    return simulationPipeline_.InitAll(context);
}

void Engine::Run() {
    loop_.Run(
        config_,
        [this](const FrameContext& frame) {
            simulationPipeline_.TickAll(frame);
        },
        [this](const FrameContext& frame) {
            renderer_.Tick(frame);
        });
}

void Engine::Shutdown() {
    simulationPipeline_.ShutdownAll();
    renderer_.Shutdown();
    std::cout << "[Engine] Shutdown\n";
}
