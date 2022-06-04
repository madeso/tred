#pragma once

#include <memory>



#include "tred/input/unitdiscovery.h"
#include "tred/input/axistype.h"


namespace input
{


struct Director;
struct ActiveUnit;
struct Converter;
struct UnitSetup;
struct Platform;


struct UnitDef
{
    virtual ~UnitDef();

    virtual std::optional<std::string> validate_key(int key) = 0;
    virtual std::optional<std::string> validate_axis(AxisType type, int target, int axis) = 0;

    virtual bool is_considered_joystick() = 0;
    virtual bool can_detect(Director* director, unit_discovery discovery, UnitSetup* setup, Platform* platform) = 0;
    virtual std::unique_ptr<ActiveUnit> create(Director* director, const UnitSetup& setup) = 0;
};

}  // namespace input
