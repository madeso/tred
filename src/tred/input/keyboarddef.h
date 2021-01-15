#pragma once

#include <vector>
#include <memory>

#include "tred/input/key.h"
#include "tred/input/unitdef.h"
#include "tred/input/bind.h"


namespace input::config
{
struct KeyboardDef;
}


namespace input
{


struct InputActionMap;
struct ActiveUnit;
struct InputDirector;

struct KeyboardDef : public UnitDef
{
    KeyboardDef(const config::KeyboardDef& data);

    bool IsConsideredJoystick() override;
    bool CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup* setup, Platform* platform) override;
    std::unique_ptr<ActiveUnit> Create(InputDirector* director, const UnitSetup& setup) override;

    Key detection_key;
};


}  // namespace input
