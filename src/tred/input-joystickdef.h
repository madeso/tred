#pragma once

#include <vector>

#include "tred/input-unitdef.h"
#include "tred/input-binddef.h"
#include "tred/input-hataxis.h"


namespace config
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
    JoystickDef(const config::JoystickDef& data, const InputActionMap& map);

    std::shared_ptr<ActiveUnit> Create(InputDirector* director, BindMap* map);

    std::vector<BindDef<int>> axes;
    std::vector<BindDef<int>> buttons;
    std::vector<BindDef<HatAxis>> hats;
};


}  // namespace input
