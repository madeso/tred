#pragma once

#include <memory>
#include <map>

#include "tred/input/platform.h"

namespace input
{


struct InputDirector;
struct ActiveUnit;
struct Converter;


struct UnitSetup
{
    std::map<int, JoystickId> joysticks;
};


struct UnitDef
{
    virtual ~UnitDef();

    virtual std::unique_ptr<ActiveUnit> Create(InputDirector* director, const UnitSetup& setup, Converter* converter) = 0;
};

}  // namespace input
