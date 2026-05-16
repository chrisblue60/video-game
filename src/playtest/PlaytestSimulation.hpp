#pragma once

#include "playtest/PlaytestState.hpp"

struct PlaytestInput {
    bool moveForward = false;
    bool moveBackward = false;
    bool moveLeft = false;
    bool moveRight = false;
    bool sprint = false;
    bool jump = false;
    bool fire = false;
    bool reload = false;
    bool resetRound = false;
    float mouseDeltaX = 0.0F;
    float mouseDeltaY = 0.0F;
};

class PlaytestSimulation {
public:
    static void Step(PlaytestState& state, const PlaytestInput& input, float fixedDtSeconds);
};
