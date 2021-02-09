#pragma once

#include <vector>

#include "tred/input/unitdef.h"
#include "tred/input/bind.h"
#include "tred/input/hataxis.h"


namespace input::config
{
struct GamecontrollerDef;
}


namespace input
{

struct InputDirector;
struct ActiveUnit;
struct InputActionMap;
struct Platform;


struct GamecontrollerDef : public UnitDef
{
    GamecontrollerDef(int index, const config::GamecontrollerDef& data);

    std::optional<std::string> ValidateKey(int key) override;
    std::optional<std::string> ValidateAxis(AxisType type, int target, int axis) override;

    bool IsConsideredJoystick() override;
    bool CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup* setup, Platform* platform) override;
    std::unique_ptr<ActiveUnit> Create(InputDirector* director, const UnitSetup& setup) override;

    int index;
};


}  // namespace input
