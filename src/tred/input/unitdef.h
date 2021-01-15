#pragma once

#include <memory>

#include "tred/input/unitdiscovery.h"


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

    virtual bool IsConsideredJoystick() = 0;
    virtual bool CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup* setup, Platform* platform) = 0;
    virtual std::unique_ptr<ActiveUnit> Create(InputDirector* director, const UnitSetup& setup) = 0;
};

}  // namespace input
