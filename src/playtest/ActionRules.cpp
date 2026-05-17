#include "playtest/ActionRules.hpp"

ActionResult ActionRules::Validate(const PlaytestState& state, const ActionRequest& request) {
    switch (request.action) {
        case ActionType::Shoot:
            if (state.weapon.ammoInMag <= 0) return {false, "Cannot shoot: empty magazine."};
            return {true, "Shoot accepted."};
        case ActionType::Reload:
            if (state.weapon.ammoInMag >= 30) return {false, "Reload ignored: magazine full."};
            if (state.weapon.reserveAmmo <= 0) return {false, "Reload ignored: no reserve ammo."};
            return {true, "Reload accepted."};
        case ActionType::PlaceStructure:
            if (!state.worldRules.inBuildParcel) return {false, "Build denied: outside parcel."};
            if (state.worldRules.buildCredits <= 0) return {false, "Build denied: no credits."};
            return {true, "Build placement accepted."};
        case ActionType::Interact:
            return {true, "Interact accepted."};
        default:
            return {true, "Action accepted."};
    }
}
