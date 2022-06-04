#include "tred/input/unitdef.keyboard.h"

#include "tred/assert.h"
#include <stdexcept>



#include "tred/input/config.h"
#include "tred/input/activeunit.keyboard.h"
#include "tred/input/director.h"
#include "tred/input/index.h"


namespace input
{

KeyboardUnitDef::KeyboardUnitDef(const config::KeyboardDefinition& data)
    : detection_key(data.detection_key)
{
}


std::optional<std::string> KeyboardUnitDef::validate_key(int key)
{
    if(key <= to_index(KeyboardKey::unbound))
    {
        return fmt::format("Invalid bind to unbound: {}", key);
    }

    if(key > to_index(KeyboardKey::sleep))
    {
        return fmt::format("Invalid bind to invalid key: {}", key);
    }

    return std::nullopt;
}


std::optional<std::string> KeyboardUnitDef::validate_axis(AxisType, int, int)
{
    return "Keyboard doesn't support axis binds";
}


bool KeyboardUnitDef::is_considered_joystick()
{
    return false;
}


bool KeyboardUnitDef::can_detect(Director* director, unit_discovery discovery, UnitSetup*, Platform*)
{
    if(discovery == unit_discovery::find_highest)
    {
        return true;
    }

    return director->was_just_pressed(detection_key);
}


std::unique_ptr<ActiveUnit> KeyboardUnitDef::create(Director* director, const UnitSetup&)
{
    ASSERT(director);

    return std::make_unique<KeyboardActiveUnit>(director);
}


}
