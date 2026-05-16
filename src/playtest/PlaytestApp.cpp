#include "playtest/PlaytestApp.hpp"

#include <iostream>

#if __has_include(<SDL2/SDL.h>)
#include <SDL2/SDL.h>
#define HAVE_SDL2 1
#else
#define HAVE_SDL2 0
#endif

#if HAVE_SDL2
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
    for (const auto& target : state.targets) if (target.alive) ++alive;
    return alive;
}

void RenderScene(SDL_Renderer* renderer, const PlaytestState& state) {
    SDL_SetRenderDrawColor(renderer, state.clock.IsNight() ? 15 : 80, state.clock.IsNight() ? 18 : 130,
                           state.clock.IsNight() ? 30 : 190, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 35, 120, 35, 255);
    SDL_Rect ground{0, kHeight / 2, kWidth, kHeight / 2};
    SDL_RenderFillRect(renderer, &ground);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, kWidth / 2 - 8, kHeight / 2, kWidth / 2 + 8, kHeight / 2);
    SDL_RenderDrawLine(renderer, kWidth / 2, kHeight / 2 - 8, kWidth / 2, kHeight / 2 + 8);

    std::ostringstream title;
    title << "Playtest | Pos(" << state.player.x << ", " << state.player.y << ", " << state.player.z
          << ") Vel(" << state.player.vx << ", " << state.player.vy << ", " << state.player.vz
          << ") TOD=" << state.clock.TimeOfDayHours() << "h"
          << " Ammo=" << state.weapon.ammoInMag << "/" << state.weapon.reserveAmmo
          << " Shots=" << state.weapon.shotsFired
          << " Hits=" << state.combat.targetsHit
          << " Alive=" << AliveTargets(state)
          << " Round=" << state.combat.roundTimeSeconds
          << " Best=" << state.combat.bestRoundSeconds;
    SDL_Window* window = SDL_RenderGetWindow(renderer);
    SDL_SetWindowTitle(window, title.str().c_str());
    SDL_RenderPresent(renderer);
}
}  // namespace
#endif

int PlaytestApp::Run() {
#if !HAVE_SDL2
    std::cerr << "SDL2 headers not found. Install libsdl2-dev to run playtest mode.\n";
    return 1;
#else
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) return 1;
    SDL_Window* window = SDL_CreateWindow("Playtest", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, kWidth, kHeight, SDL_WINDOW_SHOWN);
    if (!window) return 1;
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return 1;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    PlaytestState state{};
    state.clock.SetTimeOfDayHours(8.0);

    bool running = true;
    while (running) {
        float mouseDx = 0.0F, mouseDy = 0.0F;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            else if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
            else if (event.type == SDL_MOUSEMOTION) {
                mouseDx += static_cast<float>(event.motion.xrel);
                mouseDy += static_cast<float>(event.motion.yrel);
            }
        }
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        PlaytestInput input = BuildInput(keys, mouseDx, mouseDy);
        PlaytestSimulation::Step(state, input, kFixedDt);
        RenderScene(renderer, state);
        SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
#endif
}
