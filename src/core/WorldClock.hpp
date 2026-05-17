#pragma once

class WorldClock {
public:
    void SetTimeOfDayHours(double hours);
    void Advance(double fixedDeltaSeconds, double worldHoursPerRealSecond);

    double TimeOfDayHours() const;
    double NormalizedDayProgress() const;
    bool IsNight() const;

private:
    double timeOfDayHours_ = 8.0;
};
