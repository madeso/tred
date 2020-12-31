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


struct JoystickDef : public UnitDef
{
    JoystickDef(const config::JoystickDef& data, ConverterDef* converter);

    std::unique_ptr<ActiveUnit> Create(InputDirector* director);

    std::vector<BindDef<int>> axes;
    std::vector<BindDef<int>> buttons;
    std::vector<BindDef<HatAxis>> hats;
};


}  // namespace input
