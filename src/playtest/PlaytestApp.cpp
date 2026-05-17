#include "playtest/PlaytestApp.hpp"

#include <iostream>

#if __has_include(<SDL3/SDL.h>)
#include <SDL3/SDL.h>
#define HAVE_SDL 1
#define SDL_VER 3
#elif __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#define HAVE_SDL 1
#define SDL_VER 2
#else
#define HAVE_SDL 0
#endif

#if HAVE_SDL
#include <algorithm>
#include <cmath>
#include <cstring>
#include <sstream>
#include <string>

#include "playtest/PlaytestSimulation.hpp"
#include "playtest/VisualRules.hpp"

namespace {
constexpr int kWidth = 1280;
constexpr int kHeight = 720;
constexpr float kFixedDt = 1.0F / 60.0F;

PlaytestInput BuildInput(const Uint8* keys, float mouseDx, float mouseDy) {
    PlaytestInput input{};
    input.moveForward = keys[SDL_SCANCODE_W] != 0;
    input.moveBackward = keys[SDL_SCANCODE_S] != 0;
    input.moveLeft = keys[SDL_SCANCODE_A] != 0;
    input.moveRight = keys[SDL_SCANCODE_D] != 0;
    input.sprint = keys[SDL_SCANCODE_LSHIFT] != 0;
    input.jump = keys[SDL_SCANCODE_SPACE] != 0;
    input.fire = keys[SDL_SCANCODE_LCTRL] != 0 || keys[SDL_SCANCODE_RCTRL] != 0;
    input.reload = keys[SDL_SCANCODE_R] != 0;
    input.resetRound = keys[SDL_SCANCODE_T] != 0;
    input.interact = keys[SDL_SCANCODE_E] != 0;
    input.placeStructure = keys[SDL_SCANCODE_B] != 0;
    input.mouseDeltaX = mouseDx;
    input.mouseDeltaY = mouseDy;
    return input;
}

int AliveTargets(const PlaytestState& state) {
    int alive = 0;
    for (const auto& target : state.targets) {
        if (target.alive) {
            ++alive;
        }
    }
    return alive;
}


float AccuracyPercent(const PlaytestState& state) {
    if (state.weapon.shotsFired <= 0) return 0.0f;
    return (100.0f * static_cast<float>(state.weapon.shotsHit)) / static_cast<float>(state.weapon.shotsFired);
}
void DrawProjectedQuad(SDL_Renderer* renderer, const PlaytestState& state, float x, float y, float z, float halfW, float h) {
    const ScreenPoint p1 = VisualRules::ProjectPoint(state, kWidth, kHeight, x - halfW, y + h, z);
    const ScreenPoint p2 = VisualRules::ProjectPoint(state, kWidth, kHeight, x + halfW, y + h, z);
    const ScreenPoint p3 = VisualRules::ProjectPoint(state, kWidth, kHeight, x + halfW, y, z);
    const ScreenPoint p4 = VisualRules::ProjectPoint(state, kWidth, kHeight, x - halfW, y, z);
    if (!p1.visible || !p2.visible || !p3.visible || !p4.visible) return;

    SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
    SDL_RenderDrawLine(renderer, p2.x, p2.y, p3.x, p3.y);
    SDL_RenderDrawLine(renderer, p3.x, p3.y, p4.x, p4.y);
    SDL_RenderDrawLine(renderer, p4.x, p4.y, p1.x, p1.y);
}

void RenderGrid(SDL_Renderer* renderer, const PlaytestState& state, int horizon) {
    SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);

    const float lateral = std::fmod(state.player.x * 35.0f, 80.0f);
    for (int x = -80; x <= kWidth + 80; x += 80) {
        SDL_RenderDrawLine(renderer, static_cast<int>(x + lateral), horizon, kWidth / 2, kHeight);
    }

    for (int i = 1; i <= 16; ++i) {
        const float z = i * 3.0f + std::fmod(state.player.z, 3.0f);
        const int y = horizon + static_cast<int>(4200.0f / (z + 8.0f));
        SDL_RenderDrawLine(renderer, 0, y, kWidth, y);
    }
}

void RenderTargets(SDL_Renderer* renderer, const PlaytestState& state) {
    for (const auto& target : state.targets) {
        const ScreenPoint basePoint = VisualRules::ProjectPoint(state, kWidth, kHeight, target.x, target.y, target.z);
        if (!basePoint.visible) {
            continue;
        }
        const float depth = VisualRules::CameraDepth(state, target.x, target.z);
        if (depth <= 0.1f) continue;
        const float scale = VisualRules::PerspectiveScale(depth);
        const int halfW = static_cast<int>(8.0f * scale);
        const int height = static_cast<int>(32.0f * scale);

        if (!target.alive) {
            if (target.hitFlashSeconds <= 0.0f) {
                continue;
            }
            SDL_SetRenderDrawColor(renderer, 255, 220, 90, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 240, 70, 70, 255);
        }
        DrawProjectedQuad(renderer, state, target.x, target.y, target.z, 0.23f, 1.8f);

        // Drop shadow: farther targets cast lighter/smaller shadows to help depth reading.
        const int shadowAlpha = static_cast<int>(std::clamp(180.0f - depth * 8.0f, 40.0f, 180.0f));
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, shadowAlpha);
        const ScreenPoint shL = VisualRules::ProjectPoint(state, kWidth, kHeight, target.x - 0.35f, 0.02f, target.z);
        const ScreenPoint shR = VisualRules::ProjectPoint(state, kWidth, kHeight, target.x + 0.35f, 0.02f, target.z);
        if (shL.visible && shR.visible) {
            SDL_RenderDrawLine(renderer, shL.x, shL.y, shR.x, shR.y);
            SDL_RenderDrawLine(renderer, shL.x, shL.y + 1, shR.x, shR.y + 1);
        }

        // Center marker scales with distance.
        SDL_SetRenderDrawColor(renderer, 255, 245, 210, 255);
        SDL_RenderDrawLine(renderer, basePoint.x - halfW, basePoint.y - height / 2, basePoint.x + halfW, basePoint.y - height / 2);
    }
}

void RenderWorldObjects(SDL_Renderer* renderer, const PlaytestState& state) {
    for (const auto& object : state.worldObjects) {
        const ScreenPoint point = VisualRules::ProjectPoint(state, kWidth, kHeight, object.x, object.y, object.z);
        if (!point.visible) continue;
        const float depth = VisualRules::CameraDepth(state, object.x, object.z);
        const float scale = VisualRules::PerspectiveScale(depth);
        const int markerHalf = static_cast<int>(10.0f * scale);
        const float dx = state.player.x - object.x;
        const float dz = state.player.z - object.z;
        const float d2 = dx * dx + dz * dz;
        const bool inRange = d2 <= object.interactionRadius * object.interactionRadius;
        if (object.hitFlashSeconds > 0.0f) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        } else if (inRange) {
            SDL_SetRenderDrawColor(renderer, 90, 255, 120, 255);
        } else if (object.interaction == InteractionType::LibraryTerminal) {
            SDL_SetRenderDrawColor(renderer, 90, 170, 255, 255);
        } else if (object.interaction == InteractionType::BuildParcel) {
            SDL_SetRenderDrawColor(renderer, 255, 180, 80, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 220, 220, 220, 255);
        }
        SDL_Rect marker{point.x - markerHalf, point.y - markerHalf * 2, markerHalf * 2, markerHalf * 2};
        SDL_RenderDrawRect(renderer, &marker);
    }
}


void DrawDigit(SDL_Renderer* renderer, int x, int y, int scale, int digit) {
    static constexpr bool seg[10][7] = {
        {1,1,1,1,1,1,0}, {0,1,1,0,0,0,0}, {1,1,0,1,1,0,1}, {1,1,1,1,0,0,1}, {0,1,1,0,0,1,1},
        {1,0,1,1,0,1,1}, {1,0,1,1,1,1,1}, {1,1,1,0,0,0,0}, {1,1,1,1,1,1,1}, {1,1,1,1,0,1,1}
    };
    auto line=[&](int x1,int y1,int x2,int y2){ SDL_RenderDrawLine(renderer,x1,y1,x2,y2); };
    int w=6*scale,h=10*scale;
    if(seg[digit][0]) line(x,y,x+w,y);
    if(seg[digit][1]) line(x+w,y,x+w,y+h/2);
    if(seg[digit][2]) line(x+w,y+h/2,x+w,y+h);
    if(seg[digit][3]) line(x,y+h,x+w,y+h);
    if(seg[digit][4]) line(x,y+h/2,x,y+h);
    if(seg[digit][5]) line(x,y,x,y+h/2);
    if(seg[digit][6]) line(x,y+h/2,x+w,y+h/2);
}

void DrawNumber(SDL_Renderer* renderer, int x, int y, int scale, int value) {
    if (value < 0) value = 0;
    std::string s = std::to_string(value);
    for (size_t i = 0; i < s.size(); ++i) DrawDigit(renderer, x + static_cast<int>(i) * (8 * scale), y, scale, s[i]-'0');
}

void DrawHudOverlay(SDL_Renderer* renderer, const PlaytestState& state) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
    SDL_Rect panel{16, 16, 420, 170};
    SDL_RenderFillRect(renderer, &panel);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &panel);

    // Row 1: Hits / Alive
    DrawNumber(renderer, 30, 30, 2, state.combat.targetsHit);
    DrawNumber(renderer, 130, 30, 2, AliveTargets(state));

    // Row 2: Ammo current / reserve
    DrawNumber(renderer, 30, 62, 2, state.weapon.ammoInMag);
    DrawNumber(renderer, 130, 62, 2, state.weapon.reserveAmmo);

    // Row 3: Recoil / Accuracy
    DrawNumber(renderer, 30, 94, 2, static_cast<int>(state.camera.recoilPitchDeg * 10.0f));
    DrawNumber(renderer, 130, 94, 2, static_cast<int>(AccuracyPercent(state)));

    // small bars
    SDL_Rect accBarBg{230, 94, 100, 12};
    SDL_RenderDrawRect(renderer, &accBarBg);
    SDL_SetRenderDrawColor(renderer, 40, 220, 90, 255);
    SDL_Rect accBar{231, 95, static_cast<int>(std::min(98.0f, AccuracyPercent(state) * 0.98f)), 10};
    SDL_RenderFillRect(renderer, &accBar);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // World-building state: credits / placed / parcel state
    DrawNumber(renderer, 30, 126, 2, state.worldRules.buildCredits);
    DrawNumber(renderer, 130, 126, 2, state.worldRules.placedStructures);
    SDL_SetRenderDrawColor(renderer, state.worldRules.inBuildParcel ? 50 : 220,
                           state.worldRules.inBuildParcel ? 220 : 60, 60, 255);
    SDL_Rect parcelState{230, 126, state.worldRules.inBuildParcel ? 36 : 18, 12};
    SDL_RenderFillRect(renderer, &parcelState);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    // last interaction indicator bar (non-text fallback)
    SDL_Rect msg{30, 150, std::min(360, static_cast<int>(std::strlen(state.worldRules.lastInteraction) * 3)), 6};
    SDL_RenderFillRect(renderer, &msg);
}

void RenderScene(SDL_Renderer* renderer, const PlaytestState& state) {
    const int horizon = static_cast<int>(kHeight * 0.5f - (state.camera.pitchDeg + state.camera.recoilPitchDeg) * 2.2f);

    SDL_SetRenderDrawColor(renderer, state.clock.IsNight() ? 15 : 80, state.clock.IsNight() ? 18 : 130,
                           state.clock.IsNight() ? 30 : 190, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 35, 120, 35, 255);
    SDL_Rect ground{0, horizon, kWidth, kHeight - horizon};
    SDL_RenderFillRect(renderer, &ground);

    // Soft horizon haze for better scale and space cues.
    SDL_SetRenderDrawColor(renderer, 180, 200, 220, state.clock.IsNight() ? 30 : 55);
    SDL_Rect haze{0, horizon - 18, kWidth, 46};
    SDL_RenderFillRect(renderer, &haze);

    RenderGrid(renderer, state, horizon);
    RenderTargets(renderer, state);
    RenderWorldObjects(renderer, state);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, kWidth / 2 - 8, kHeight / 2, kWidth / 2 + 8, kHeight / 2);
    SDL_RenderDrawLine(renderer, kWidth / 2, kHeight / 2 - 8, kWidth / 2, kHeight / 2 + 8);

    DrawHudOverlay(renderer, state);
    SDL_RenderPresent(renderer);
}
}  // namespace
#endif

int PlaytestApp::Run() {
#if !HAVE_SDL
    std::cerr << "SDL headers not found. Install SDL2 or SDL3 development package to run playtest mode.\n";
    return 1;
#else
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
        return 1;
    }

#if SDL_VER == 3
    SDL_Window* window = SDL_CreateWindow("Playtest", kWidth, kHeight, 0);
    SDL_Renderer* renderer = window ? SDL_CreateRenderer(window, nullptr) : nullptr;
#else
    SDL_Window* window = SDL_CreateWindow("Playtest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          kWidth, kHeight, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = window ? SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED) : nullptr;
#endif

    if (!window || !renderer) {
        return 1;
    }

    SDL_SetRelativeMouseMode(SDL_TRUE);

    PlaytestState state{};
    state.clock.SetTimeOfDayHours(8.0);

    bool running = true;
    while (running) {
        float mouseDx = 0.0f;
        float mouseDy = 0.0f;

        SDL_Event event;
        while (SDL_PollEvent(&event)) {
#if SDL_VER == 3
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            } else if (event.type == SDL_EVENT_KEY_DOWN && event.key.key == SDLK_ESCAPE) {
                running = false;
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
#else
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            } else if (event.type == SDL_MOUSEMOTION) {
#endif
                mouseDx += static_cast<float>(event.motion.xrel);
                mouseDy += static_cast<float>(event.motion.yrel);
            }
        }

        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        PlaytestInput input = BuildInput(keys, mouseDx, mouseDy);
        PlaytestSimulation::Step(state, input, kFixedDt);

        RenderScene(renderer, state);
#if SDL_VER == 3
        SDL_DelayNS(16 * 1000 * 1000);
#else
        SDL_Delay(16);
#endif
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
#endif
}
