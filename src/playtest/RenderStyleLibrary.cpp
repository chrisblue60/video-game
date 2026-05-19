#include "playtest/RenderStyleLibrary.hpp"

RenderPalette RenderStyleLibrary::DayPalette() { return RenderPalette{}; }

RenderPalette RenderStyleLibrary::NightPalette() {
    RenderPalette p{};
    p.skyR = 15;
    p.skyG = 18;
    p.skyB = 30;
    p.groundR = 26;
    p.groundG = 55;
    p.groundB = 28;
    return p;
}

CharacterRenderProfile RenderStyleLibrary::DefaultTargetProfile() { return CharacterRenderProfile{}; }
