#pragma once

#include <vector>

#include "tred/input/unitdef.h"
#include "tred/input/bind.h"
#include "tred/input/hataxis.h"


namespace input::config
{
struct JoystickDef;
}


namespace input
{

struct InputDirector;
struct ActiveUnit;
struct InputActionMap;
struct Platform;


struct JoystickDef : public UnitDef
{
    JoystickDef(int index, const config::JoystickDef& data);

    std::optional<std::string> ValidateKey(int key) override;

    bool IsConsideredJoystick() override;
    bool CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup* setup, Platform* platform) override;
    std::unique_ptr<ActiveUnit> Create(InputDirector* director, const UnitSetup& setup) override;

    int index;
    int start_button;
    std::string unit;
};


}  // namespace input
