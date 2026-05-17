# Video Game Engine Prototype

Initial scaffold for a custom open-world FPS engine targeting Xbox and PC.

## Current milestone
- Core lifecycle and config-driven fixed-timestep loop
- Subsystem pipeline with renderer/physics/world-sim stubs
- ECS-style world foundation with `Entity`, `Transform`, `Velocity`, `Gravity`, and `Collider` components
- Basic kinematic + gravity integration in Physics subsystem with collider-aware ground contact clamp
- SDL playtest app supporting SDL3-first detection with SDL2 fallback
- Pseudo-3D debug renderer with projected targets, parallax ground grid, pitch-shift horizon, and hit flash feedback
- Recoil kick/recovery integrated into aiming and simulation
- Automated tests for world clock, config loader, subsystem pipeline, world storage, and playtest simulation logic

## Build + Playtest on Windows (recommended)
```powershell
cmake --preset windows-vs2022 -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build --preset windows-vs2022-debug
ctest --preset windows-vs2022-debug
.\build-vs2022\Debug\engine_playtest.exe
```

## Playtest controls
- `W/A/S/D`: move
- `LShift`: sprint
- `Space`: jump
- `LCTRL`/`RCTRL`: fire
- `R`: reload
- `T`: reset round targets/timer
- Mouse: free-look
- `Esc`: quit

## Playtest verification checklist
When you run `engine_playtest`, verify the following visually:
1. **WASD movement feedback**
   - Ground grid and target projections should shift with movement.
2. **Look direction feedback**
   - Mouse look should rotate projected targets left/right and shift horizon up/down.
3. **Jump/vertical motion feedback**
   - Press `Space`; horizon and target vertical placement should respond while airborne.
4. **Target aiming/hit feedback**
   - Aim crosshair at red target markers and fire.
   - On hit: target flashes yellow briefly, then disappears, `Hits` increases, `Alive` decreases.
5. **Recoil behavior**
   - Firing should kick aim upward (`Recoil` value increases) and recover smoothly over time.
6. **Round progression**
   - Clear all targets and verify round timer resets and `Best` is tracked.
7. **Reload/ammo flow**
   - Fire until magazine drops, press `R`, confirm ammo transfers from reserve.

## Non-Windows quick build
```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Rule Libraries (World Stack)
- `src/playtest/VisualRules.*`: camera projection, perspective scaling, and near-camera clamped projection for stable world-space rendering.
- `src/playtest/BulletRules.*`: first-hit resolution, occlusion priority, and penetration-aware shot handling.
- `src/playtest/WorldProperties.hpp`: shared property vocabulary (`MaterialType`, `CollisionProperties`, `InteractionProperties`, `CombatProperties`) used to scale world behaviors safely.

### Property usage guidance
- `CollisionProperties`: Use for movement blocking and projectile blocking semantics; `radius/height` should represent the same physical envelope used by rendering and interaction.
- `InteractionProperties`: Use for generic action routing (`interact`, future `grab`, `use`, `build`).
- `CombatProperties`: Use to determine damageability/targetability and penetrable behavior (`shootThrough`) for projectiles.
- `MaterialType`: Use as a high-level classification for future rule branches (audio, VFX, penetration, ricochet, tools, destruction).

This stack is designed to expand toward broad world simulation without re-implementing behavior per object type.
