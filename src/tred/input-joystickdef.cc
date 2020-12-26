#include "tred/input-joystickdef.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/log.h"
#include "tred/input-config.h"
#include "tred/input-dummyactiveunit.h"
#include "tred/input-taxisbind.h"
#include "tred/input-trangebind.h"
#include "tred/input-actionmap.h"
#include "tred/input-bindmap.h"
#include "tred/input-commondef.h"
#include "tred/input-joystickactiveunit.h"


namespace input
{


JoystickDef::JoystickDef(const config::JoystickDef& data, const InputActionMap&)
{
    for (const auto& d: data.axis)
    {
        const int axis = d.axis;
        if (axis < 0)
        {
            LOG_ERROR("Invalid joystick axis for {} action", d.bindname);
        }
        else
        {
            axes.push_back(BindDef<int>(d.bindname, axis, d));
        }
    }
    for (const auto& d: data.button)
    {
        const int key = d.button;

        if (key < 0)
        {
            LOG_ERROR("Invalid joystick button for the {} action", d.bindname);
        }
        else
        {
            buttons.push_back(BindDef<int>(d.bindname, key, d));
        }
    }
    for (const auto& d: data.hat)
    {
        const int hat = d.hat;
        if (hat < 0)
        {
            LOG_ERROR("Invalid joystick hat for the {} action", d.bindname);
        }
        else
        {
            hats.push_back(BindDef<HatAxis>(d.bindname, HatAxis(hat, d.axis), d));
        }
    }
}


std::shared_ptr<ActiveUnit> JoystickDef::Create(InputDirector* director, BindMap* map)
{
    assert(director);
    assert(map);

    /// @todo fix the joystick number
    int js = 0;

    std::vector<std::shared_ptr<TAxisBind<int>>> axisbinds = CreateBinds<TAxisBind<int>, int>(axes, map);
    std::vector<std::shared_ptr<TRangeBind<int>>> buttonbinds = CreateBinds<TRangeBind<int>, int>(buttons, map);
    std::vector<std::shared_ptr<TAxisBind<HatAxis>>> hatbinds = CreateBinds<TAxisBind<HatAxis>, HatAxis>(hats, map);

    std::shared_ptr<ActiveUnit> unit(new JoystickActiveUnit(js, director, axisbinds, buttonbinds, hatbinds));
    return unit;
}


}  // namespace input
