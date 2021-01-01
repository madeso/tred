#include "tred/input/joystickdef.h"

#include <cassert>
#include <string>
#include "fmt/format.h"

#include "tred/log.h"
#include "tred/input/config.h"
#include "tred/input/bind.h"
#include "tred/input/actionmap.h"
#include "tred/input/joystickactiveunit.h"


namespace input
{


JoystickDef::JoystickDef(const config::JoystickDef& data, ConverterDef* converter)
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
            axes.push_back(BindDef<int>(d.bindname, axis, d, converter));
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
            buttons.push_back(BindDef<int>(d.bindname, key, d, converter));
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
            hats.push_back(BindDef<HatAxis>(d.bindname, HatAxis(hat, d.axis), d, converter));
        }
    }
}


std::unique_ptr<ActiveUnit> JoystickDef::Create(InputDirector* director, Converter* converter)
{
    assert(director);

    /// @todo fix the joystick number
    int js = 0;
    
    return std::make_unique<JoystickActiveUnit>(js, director, converter, axes, buttons, hats);
}


}  // namespace input
