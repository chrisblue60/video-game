#pragma once

#include "playtest/PlaytestState.hpp"

// High-level player/world action taxonomy. These actions map to current controls
// and future capabilities (e.g., grabbing, crafting, operating vehicles/devices).
enum class ActionType {
    None,
    Shoot,
    Reload,
    Interact,
    PlaceStructure,
    Grab,
    Drop,
    UseTool,
};

// Unified action request payload for rule dispatch.
struct ActionRequest {
    ActionType action = ActionType::None;
    bool pressed = false;   // Edge/trigger action.
    bool held = false;      // Continuous action.
    float range = 0.0F;     // Optional action range hint.
};

// Generic action outcome model for downstream systems (UI, analytics, replay).
struct ActionResult {
    bool accepted = false;
    const char* message = "";
};

class ActionRules {
public:
    // Resolve high-level action eligibility based on current world and player state.
    // This function is intentionally conservative in the prototype and can later
    // route to specialized handlers (combat/building/inventory/vehicles/etc.).
    static ActionResult Validate(const PlaytestState& state, const ActionRequest& request);
};
