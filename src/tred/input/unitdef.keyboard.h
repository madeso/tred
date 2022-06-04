#pragma once


#include <memory>

#include "tred/input/key.h"
#include "tred/input/unitdef.h"
#include "tred/input/bind.h"


namespace input::config
{
struct KeyboardDefinition;
}


namespace input
{


struct ActionMap;
struct ActiveUnit;
struct Director;

struct KeyboardUnitDef : public UnitDef
{
    KeyboardUnitDef(const config::KeyboardDefinition& data);

    std::optional<std::string> validate_key(int key) override;
    std::optional<std::string> validate_axis(AxisType type, int target, int axis) override;

    bool is_considered_joystick() override;
    bool can_detect(Director* director, unit_discovery discovery, UnitSetup* setup, Platform* platform) override;
    std::unique_ptr<ActiveUnit> create(Director* director, const UnitSetup& setup) override;

    KeyboardKey detection_key;
};


}  // namespace input
