#pragma once

#include <vector>

#include "tred/input/unitdef.h"
#include "tred/input/axis.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"


namespace input::config
{
struct MouseDefinition;
}


namespace input
{


struct ActionMap;


struct MouseUnitDef : public UnitDef
{
    MouseUnitDef(const config::MouseDefinition& data);

    std::optional<std::string> validate_key(int key) override;
    std::optional<std::string> validate_axis(AxisType type, int target, int axis) override;

    bool is_considered_joystick() override;
    bool can_detect(Director* director, unit_discovery discovery, UnitSetup* setup, Platform* platform) override;
    std::unique_ptr<ActiveUnit> create(Director* director, const UnitSetup& setup) override;

    MouseButton detection_button;
};


}
