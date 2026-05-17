#include "playtest/PlaytestSimulation.hpp"

#include <cmath>
#include <iostream>

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
    { PlaytestState s{}; s.weapon.ammoInMag=5; s.weapon.reserveAmmo=10; PlaytestInput i{}; i.reload=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.weapon.ammoInMag!=15||s.weapon.reserveAmmo!=0) return Fail("reload"); }
    { PlaytestState s{}; PlaytestInput i{}; i.fire=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.combat.targetsHit<1) return Fail("hitscan"); }
    { PlaytestState s{}; PlaytestInput i{}; i.resetRound=true; PlaytestSimulation::Step(s,i,1.0F/60.0F); if(s.targets.size()!=3) return Fail("reset spawns targets"); }

    std::cout << "[PASS] PlaytestSimulation tests\n";
    return 0;
}
