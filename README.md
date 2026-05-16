# Video Game Engine Prototype

Initial scaffold for a custom open-world FPS engine targeting Xbox and PC.

## Current milestone
- Core lifecycle and config-driven fixed-timestep loop
- Subsystem pipeline with renderer/physics/world-sim stubs
- ECS-style world foundation with `Entity`, `Transform`, `Velocity`, `Gravity`, and `Collider` components
- Basic kinematic + gravity integration in Physics subsystem with collider-aware ground contact clamp
- Optional SDL playtest app with keyboard/mouse controls, free-look camera, movement, jumping, sprinting, and firing/reload simulation
- Debug HUD in window title showing position, velocity, time-of-day, ammo, shots, hit progress, and round timers
- Playtest combat prototype with seeded targets, hitscan scoring, auto-reset rounds, and best-time tracking
- Automated tests for world clock, config loader, subsystem pipeline, world storage, and playtest simulation logic

## SDL Support
- Playtest now supports **SDL3 first**, and falls back to **SDL2** automatically.
- CMake attempts SDL detection in this order:
  1. `find_package(SDL3 CONFIG)`
  2. `find_package(SDL2 CONFIG)` then module fallback

## Build + Playtest on Windows (recommended)
```powershell
cmake --preset windows-vs2022
cmake --build --preset windows-vs2022-debug
ctest --preset windows-vs2022-debug
.\build-vs2022\Debug\engine_playtest.exe
```

If runtime DLL missing, copy either `SDL3.dll` or `SDL2.dll` (matching the detected SDK) next to `engine_playtest.exe`.

## Playtest controls
- `W/A/S/D`: move
- `LShift`: sprint
- `Space`: jump
- `LCTRL`/`RCTRL`: fire
- `R`: reload
- `T`: reset round targets/timer
- Mouse: free-look
- `Esc`: quit
