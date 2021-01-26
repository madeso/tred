#pragma once

#include <memory>
#include <optional>
#include <string>

#include "tred/input/unitdiscovery.h"
#include "tred/input/axistype.h"


namespace input
{


struct InputDirector;
struct ActiveUnit;
struct Converter;
struct UnitSetup;
struct Platform;


struct UnitDef
{
    virtual ~UnitDef();

    virtual std::optional<std::string> ValidateKey(int key) = 0;
    virtual std::optional<std::string> ValidateAxis(AxisType type, int target, int axis) = 0;

    virtual bool IsConsideredJoystick() = 0;
    virtual bool CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup* setup, Platform* platform) = 0;
    virtual std::unique_ptr<ActiveUnit> Create(InputDirector* director, const UnitSetup& setup) = 0;
};

}  // namespace input
