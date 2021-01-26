#pragma once

#include <vector>

#include "tred/input/unitdef.h"
#include "tred/input/axis.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"


namespace input::config
{
struct MouseDef;
}


namespace input
{


struct InputActionMap;


struct MouseDef : public UnitDef
{
    MouseDef(const config::MouseDef& data);

    std::optional<std::string> ValidateKey(int key) override;
    std::optional<std::string> ValidateAxis(AxisType type, int target, int axis) override;

    bool IsConsideredJoystick() override;
    bool CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup* setup, Platform* platform) override;
    std::unique_ptr<ActiveUnit> Create(InputDirector* director, const UnitSetup& setup) override;

    MouseButton detection_button;
};


}  // namespace input
