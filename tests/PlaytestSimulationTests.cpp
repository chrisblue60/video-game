#include "playtest/PlaytestSimulation.hpp"
#include "playtest/ActionRules.hpp"
#include "playtest/VisualRules.hpp"

#include <cmath>
#include <iostream>
#include <string>

namespace {
bool NearlyEqual(float a, float b, float eps = 1e-4F) { return std::fabs(a - b) < eps; }
int Fail(const char* message) {
    std::cerr << "[FAIL] " << message << "\n";
    return 1;
}
}  // namespace

int main() {
    { PlaytestState s{}; PlaytestInput i{}; i.moveForward=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.player.z<=0) return Fail("forward"); }
    { PlaytestState s{}; s.player.y=0.9F; PlaytestInput i{}; i.jump=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.player.vy<=0) return Fail("jump"); }
    { PlaytestState s{}; s.player.y=10.0F; PlaytestInput i{}; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.player.vy>=0) return Fail("gravity"); }
    { PlaytestState s{}; double st=s.clock.TimeOfDayHours(); PlaytestInput i{}; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(!(s.clock.TimeOfDayHours()>st)) return Fail("clock"); }
    { PlaytestState s{}; PlaytestInput i{}; i.mouseDeltaY=99999.0F; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(!NearlyEqual(s.camera.pitchDeg,-89.0F,1e-3F)) return Fail("pitch"); }
    { PlaytestState s{}; PlaytestInput i{}; i.fire=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.weapon.ammoInMag!=29||s.weapon.shotsFired!=1) return Fail("fire"); if(s.weapon.shotsHit!=1) return Fail("shotsHit"); }
    { PlaytestState s{}; s.camera.yawDeg=5.0F; PlaytestInput i{}; i.fire=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.weapon.shotsHit!=0) return Fail("precise miss outside projected hitbox"); }
    { PlaytestState s{}; s.player.z=7.0F; s.camera.z=7.0F; PlaytestInput i{}; i.fire=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.weapon.shotsHit!=1) return Fail("close-range hit should register"); }
    { PlaytestState s{}; s.weapon.ammoInMag=5; s.weapon.reserveAmmo=10; PlaytestInput i{}; i.reload=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.weapon.ammoInMag!=15||s.weapon.reserveAmmo!=0) return Fail("reload"); }
    { PlaytestState s{}; s.weapon.ammoInMag=0; ActionResult r=ActionRules::Validate(s, ActionRequest{ActionType::Shoot,true,false,0.0F}); if(r.accepted) return Fail("action rules shoot validation"); }
    { PlaytestState s{}; PlaytestInput i{}; i.fire=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.combat.targetsHit<1) return Fail("hitscan"); }
    { PlaytestState s{}; s.targets={TargetState{0.0F,1.0F,8.0F,true},TargetState{0.0F,1.0F,12.0F,true}}; PlaytestInput i{}; i.fire=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.combat.targetsHit!=1) return Fail("single-hit only"); if(s.targets[1].alive!=true) return Fail("cannot hit two targets"); }
    { PlaytestState s{}; s.targets={TargetState{0.0F,1.0F,12.0F,true}}; s.worldObjects={WorldObject{"Blocker",0.0F,0.9F,8.0F,2.0F,1.0F,InteractionType::None,false,0.0F}}; PlaytestInput i{}; i.fire=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.targets[0].alive!=true) return Fail("object should block target hit"); }
    { PlaytestState s{}; PlaytestInput i{}; i.resetRound=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.targets.size()!=3) return Fail("reset spawns targets"); }
    { PlaytestState s{}; s.player.x=-6.0F; s.player.z=12.0F; PlaytestInput i{}; i.interact=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(std::string(s.worldRules.lastInteraction).find("Library opened")==std::string::npos) return Fail("library interact"); }
    { PlaytestState s{}; s.player.x=6.0F; s.player.z=10.0F; PlaytestInput i{}; i.placeStructure=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.worldRules.placedStructures!=1||s.worldRules.buildCredits!=3) return Fail("place structure in parcel"); }
    { PlaytestState s{}; PlaytestInput i{}; i.placeStructure=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(std::string(s.worldRules.lastInteraction).find("move into a build parcel")==std::string::npos) return Fail("prevent place outside parcel"); }
    { if(!(VisualRules::PerspectiveScale(4.0F) > VisualRules::PerspectiveScale(20.0F))) return Fail("perspective scale depth rule"); }
    { PlaytestState s{}; PlaytestInput i{}; i.moveForward=true; for(int k=0;k<120;++k) PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.player.z>7.2F) return Fail("collision with target"); }

    std::cout << "[PASS] PlaytestSimulation tests\n";
    return 0;
}
