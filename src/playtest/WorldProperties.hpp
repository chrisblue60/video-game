#pragma once

// WorldProperties is a shared library of semantic properties used by simulation rules.
// These properties are intentionally broader than the current playtest feature set so
// we can expand behavior without rewriting call sites.

enum class MaterialType {
    DefaultSolid,   // Opaque, collides, blocks bullets.
    Glass,          // Typically collides; may be shoot-through depending on policy.
    Wood,           // Default destructible candidate.
    Metal,          // Strongly blocking, non-penetrable by default.
    Water,          // Non-solid movement medium; slows and distorts.
    EnergyField,    // Scriptable behavior (block/allow depending on state).
};

struct CollisionProperties {
    bool blocksPlayer = true;      // If true, player movement resolves collision against this entity.
    bool blocksProjectiles = true; // If true, bullets/projectiles stop here unless penetration rules override.
    float radius = 0.9F;           // Broad-phase radius for quick hit/collision checks.
    float height = 1.6F;           // Visual/interaction vertical envelope.
};

struct InteractionProperties {
    bool interactable = false;     // If true, generic interact action can target this entity.
    bool grabbable = false;        // Future action library: pickup/drag/throw.
    bool buildableSurface = false; // Future world-editing/build system anchor surface.
    float useRadius = 2.0F;        // Generic interaction distance.
};

struct CombatProperties {
    bool targetable = false;       // Can be targeted by aiming systems.
    bool damageable = false;       // Can receive damage effects.
    bool shootThrough = false;     // Allows projectile continuation to possible downstream hit.
    float health = 100.0F;         // Generic health reservoir for future damage systems.
};

struct WorldEntityProperties {
    MaterialType material = MaterialType::DefaultSolid;
    CollisionProperties collision{};
    InteractionProperties interaction{};
    CombatProperties combat{};
};
