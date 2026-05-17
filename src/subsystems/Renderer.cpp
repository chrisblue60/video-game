#include "subsystems/Renderer.hpp"

#include <iostream>

const char* Renderer::Name() const {
    return "Renderer";
}

bool Renderer::Init(const SubsystemContext& context) {
    verboseLogging_ = context.config.verboseLogging;
    std::cout << "[Renderer] Init (backend interface stub ready)\n";
    return true;
}

void Renderer::Tick(const FrameContext& frame) {
    if (!verboseLogging_ && (frame.frameIndex % 120 != 0)) {
        return;
    }

    std::cout << "[Renderer] Frame=" << frame.frameIndex
              << " alpha=" << frame.interpolationAlpha << "\n";
}

void Renderer::Shutdown() {
    std::cout << "[Renderer] Shutdown\n";
}
