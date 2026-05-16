#include "core/SubsystemPipeline.hpp"

#include <iostream>
#include <string>
#include <vector>

namespace {

struct FakeSubsystem final : ISubsystem {
    explicit FakeSubsystem(std::string subsystemName, bool shouldInitSucceed, std::vector<std::string>* events)
        : name(std::move(subsystemName)), initSucceeds(shouldInitSucceed), eventLog(events) {}

    const char* Name() const override { return name.c_str(); }

    bool Init(const SubsystemContext&) override {
        eventLog->push_back(name + ":init");
        return initSucceeds;
    }

    void Tick(const FrameContext&) override {
        eventLog->push_back(name + ":tick");
    }

    void Shutdown() override {
        eventLog->push_back(name + ":shutdown");
    }

    std::string name;
    bool initSucceeds;
    std::vector<std::string>* eventLog;
};

int Fail(const char* message) {
    std::cerr << "[FAIL] " << message << "\n";
    return 1;
}

}  // namespace

int main() {
    {
        std::vector<std::string> events;
        FakeSubsystem a("A", true, &events);
        FakeSubsystem b("B", true, &events);
        SubsystemPipeline pipeline;
        pipeline.Register(&a);
        pipeline.Register(&b);

        EngineConfig cfg{};
        SubsystemContext ctx{cfg};
        if (!pipeline.InitAll(ctx)) {
            return Fail("InitAll should succeed");
        }

        FrameContext frame{};
        pipeline.TickAll(frame);
        pipeline.ShutdownAll();

        const std::vector<std::string> expected = {
            "A:init", "B:init", "A:tick", "B:tick", "B:shutdown", "A:shutdown"};
        if (events != expected) {
            return Fail("Pipeline order should be init/tick in registration order and shutdown in reverse");
        }
    }

    {
        std::vector<std::string> events;
        FakeSubsystem a("A", true, &events);
        FakeSubsystem b("B", false, &events);
        SubsystemPipeline pipeline;
        pipeline.Register(&a);
        pipeline.Register(&b);

        EngineConfig cfg{};
        SubsystemContext ctx{cfg};
        if (pipeline.InitAll(ctx)) {
            return Fail("InitAll should fail when a subsystem init fails");
        }
    }

    std::cout << "[PASS] SubsystemPipeline tests\n";
    return 0;
}
