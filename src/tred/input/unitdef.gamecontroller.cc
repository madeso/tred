#include "unitdef.gamecontroller.h"

#include "tred/assert.h"



#include "tred/log.h"
#include "tred/input/config.h"
#include "tred/input/bind.h"
#include "tred/input/key.h"
#include "tred/input/axis.h"
#include "tred/input/actionmap.h"
#include "tred/input/activeunit.gamecontroller.h"
#include "tred/input/unitsetup.h"
#include "tred/input/platform.h"
#include "tred/input/director.h"
#include "tred/input/index.h"


namespace input
{


GamecontrollerUnitDef::GamecontrollerUnitDef(int id, const config::GamecontrollerDefinition&)
    : index(id)
{
}


std::optional<std::string> GamecontrollerUnitDef::validate_key(int key)
{
    if(key <= to_index(GamecontrollerButton::invalid)
    || key > to_index(GamecontrollerButton::trigger_right))
    {
        return fmt::format("Invalid bind to invalid gamecontroller button: {}", key);
    }

    return std::nullopt;
}


std::optional<std::string> GamecontrollerUnitDef::validate_axis(AxisType type, int target, int axis)
{
    if(type != AxisType::general_axis) { return fmt::format("Only general axcis for game controllers: {}", type); }
    if(target != 0) { return fmt::format("Target needs to be 0 for controller: {}", target); }

    switch(from_index<GamecontrollerAxis>(axis))
    {
        case GamecontrollerAxis::left_x:
        case GamecontrollerAxis::left_y:
        case GamecontrollerAxis::right_x:
        case GamecontrollerAxis::right_y:
            return std::nullopt;
        default:
            return fmt::format("Invalid axis: {}", axis);
    }
}


bool GamecontrollerUnitDef::is_considered_joystick()
{
    return true;
}


bool GamecontrollerUnitDef::can_detect(Director* director, unit_discovery, UnitSetup* setup, Platform* platform)
{
    const auto potential_joysticks = platform->get_active_and_free_joysticks();
    for(auto joy: potential_joysticks)
    {
        const auto selected = setup->has_joystick(joy) == false && director->was_game_controller_start_just_pressed(joy);
        if(selected)
        {
            platform->start_using(joy, JoystickType::game_controller);
            setup->add_joystick(index, joy);
            return true;
        }
    }

    return false;
}


std::unique_ptr<ActiveUnit> GamecontrollerUnitDef::create(Director* director, const UnitSetup& setup)
{
    ASSERT(director);
    return std::make_unique<GamecontrollerActiveUnit>(setup.get_joystick(index), director);
}


}
