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


JoystickDef::JoystickDef(int id, const config::JoystickDef& data, ConverterDef* converter)
    : index(id)
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

    for (const auto& d: data.ball)
    {
        const int ball = d.hat;
        if (ball < 0)
        {
            LOG_ERROR("Invalid joystick ball for the {} action", d.bindname);
        }
        else
        {
            balls.push_back(BindDef<HatAxis>(d.bindname, HatAxis(ball, d.axis), d, converter));
        }
    }
}


std::unique_ptr<ActiveUnit> JoystickDef::Create(InputDirector* director, const UnitSetup& setup, Converter* converter)
{
    assert(director);

    auto found = setup.joysticks.find(index);
    assert(found != setup.joysticks.end());
    
    return std::make_unique<JoystickActiveUnit>(found->second, director, converter, axes, buttons, hats, balls);
}


}  // namespace input
