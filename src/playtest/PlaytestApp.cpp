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
#include <cmath>
#include <sstream>

#include "playtest/PlaytestSimulation.hpp"

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

bool ProjectPoint(const PlaytestState& state, float wx, float wy, float wz, int& sx, int& sy) {
    constexpr float pi = 3.14159265358979323846f;
    const float yaw = state.camera.yawDeg * pi / 180.0f;
    const float pitch = (state.camera.pitchDeg + state.camera.recoilPitchDeg) * pi / 180.0f;
    const float dx = wx - state.camera.x;
    const float dy = wy - state.camera.y;
    const float dz = wz - state.camera.z;

    const float cy = std::cos(yaw);
    const float syaw = std::sin(yaw);
    const float x1 = cy * dx + syaw * dz;
    const float z1 = -syaw * dx + cy * dz;
    const float y1 = dy;

    const float cp = std::cos(pitch);
    const float sp = std::sin(pitch);
    const float y2 = cp * y1 - sp * z1;
    const float z2 = sp * y1 + cp * z1;
    if (z2 <= 0.1f) {
        return false;
    }

    const float focal = 550.0f;
    sx = static_cast<int>(kWidth * 0.5f + (x1 / z2) * focal);
    sy = static_cast<int>(kHeight * 0.5f - (y2 / z2) * focal);
    return sx >= -50 && sx <= kWidth + 50 && sy >= -50 && sy <= kHeight + 50;
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
        int sx = 0;
        int sy = 0;
        if (!ProjectPoint(state, target.x, target.y, target.z, sx, sy)) {
            continue;
        }

        if (!target.alive) {
            if (target.hitFlashSeconds <= 0.0f) {
                continue;
            }
            SDL_SetRenderDrawColor(renderer, 255, 230, 80, 255);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 60, 60, 255);
        }

        SDL_Rect r{sx - 8, sy - 16, 16, 32};
        SDL_RenderDrawRect(renderer, &r);
        SDL_RenderDrawLine(renderer, sx - 10, sy, sx + 10, sy);
    }
}

void RenderScene(SDL_Renderer* renderer, const PlaytestState& state) {
    const int horizon = static_cast<int>(kHeight * 0.5f - (state.camera.pitchDeg + state.camera.recoilPitchDeg) * 2.2f);

    SDL_SetRenderDrawColor(renderer, state.clock.IsNight() ? 15 : 80, state.clock.IsNight() ? 18 : 130,
                           state.clock.IsNight() ? 30 : 190, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 35, 120, 35, 255);
    SDL_Rect ground{0, horizon, kWidth, kHeight - horizon};
    SDL_RenderFillRect(renderer, &ground);

    RenderGrid(renderer, state, horizon);
    RenderTargets(renderer, state);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, kWidth / 2 - 8, kHeight / 2, kWidth / 2 + 8, kHeight / 2);
    SDL_RenderDrawLine(renderer, kWidth / 2, kHeight / 2 - 8, kWidth / 2, kHeight / 2 + 8);

    std::ostringstream title;
    title << "Playtest | Pos(" << state.player.x << ", " << state.player.y << ", " << state.player.z
          << ") Vel(" << state.player.vx << ", " << state.player.vy << ", " << state.player.vz
          << ") TOD=" << state.clock.TimeOfDayHours() << "h"
          << " Ammo=" << state.weapon.ammoInMag << "/" << state.weapon.reserveAmmo
          << " Hits=" << state.combat.targetsHit
          << " Alive=" << AliveTargets(state)
          << " Round=" << state.combat.roundTimeSeconds
          << " Best=" << state.combat.bestRoundSeconds
          << " Recoil=" << state.camera.recoilPitchDeg;
#if SDL_VER == 3
    SDL_SetWindowTitle(SDL_GetRenderWindow(renderer), title.str().c_str());
#else
    SDL_Window* window = SDL_RenderGetWindow(renderer);
    SDL_SetWindowTitle(window, title.str().c_str());
#endif

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
