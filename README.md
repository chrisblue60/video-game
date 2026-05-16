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

## Build (Linux/macOS)
```bash
cmake -S . -B build
cmake --build build
./build/engine_prototype
```

## Build + Playtest on Windows (recommended)

### 1) Install prerequisites
- Visual Studio 2022 with **Desktop development with C++**
- CMake (3.20+)
- SDL2 development package (headers + libs + `SDL2.dll`)

### 2) Configure and build with preset
From PowerShell in repo root:
```powershell
cmake --preset windows-vs2022
cmake --build --preset windows-vs2022-debug
```

### 3) Run automated tests
```powershell
ctest --preset windows-vs2022-debug
```

### 4) Run the playtest executable
```powershell
.\build-vs2022\Debug\engine_playtest.exe
```

If `engine_playtest.exe` fails with a missing SDL runtime DLL, copy `SDL2.dll` next to the executable (`build-vs2022\Debug\`).

## Playtest controls
- `W/A/S/D`: move
- `LShift`: sprint
- `Space`: jump
- `LCTRL`/`RCTRL`: fire
- `R`: reload
- `T`: reset round targets/timer
- Mouse: free-look
- `Esc`: quit

Goal of current playtest:
- Clear all targets quickly; HUD shows `Round` timer and `Best` completion time.

## Tests (non-Windows quick path)
```bash
cd build
ctest --output-on-failure
```
