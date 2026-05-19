#pragma once

// RenderStyleLibrary centralizes visual style parameters so the renderer can
// evolve toward production-quality art direction without scattering constants.
struct RenderPalette {
    unsigned char skyR = 80;
    unsigned char skyG = 130;
    unsigned char skyB = 190;
    unsigned char groundR = 35;
    unsigned char groundG = 120;
    unsigned char groundB = 35;
};

struct CharacterRenderProfile {
    float bodyHeightMeters = 1.8F;
    float bodyHalfWidthMeters = 0.45F;
    float shoulderWidthMeters = 0.55F;
    float legLengthMeters = 0.95F;
};

class RenderStyleLibrary {
public:
    static RenderPalette DayPalette();
    static RenderPalette NightPalette();
    static CharacterRenderProfile DefaultTargetProfile();
};
