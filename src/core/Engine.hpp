#pragma once

#include "core/EngineConfig.hpp"
#include "core/GameLoop.hpp"
#include "core/SubsystemPipeline.hpp"
#include "subsystems/Physics.hpp"
#include "subsystems/Renderer.hpp"
#include "subsystems/WorldSim.hpp"

class Engine {
public:
    explicit Engine(EngineConfig config);

    bool Init();
    void Run();
    void Shutdown();

private:
    EngineConfig config_;
    Renderer renderer_;
    Physics physics_;
    WorldSim worldSim_;
    SubsystemPipeline simulationPipeline_;
    GameLoop loop_;
};
