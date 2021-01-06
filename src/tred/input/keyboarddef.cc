#include "tred/input/keyboarddef.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include "fmt/format.h"

#include "tred/input/config.h"
#include "tred/input/keyboardactiveunit.h"
#include "tred/input/director.h"


namespace input
{

KeyboardDef::KeyboardDef(const config::KeyboardDef& data, ConverterDef* converter)
    : detection_key(data.detection_key)
{
    for (const auto& d: data.binds)
    {
        keys.push_back(BindDef<Key>(d.bindname, d.key, d, converter));
    }
}


bool KeyboardDef::IsConsideredJoystick()
{
    return false;
}


bool KeyboardDef::CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup*, Platform*)
{
    if(discovery == UnitDiscovery::FindHighest)
    {
        return true;
    }

    return director->WasJustPressed(detection_key);
}


std::unique_ptr<ActiveUnit> KeyboardDef::Create(InputDirector* director, const UnitSetup&, Converter* converter)
{
    assert(director);
    
    return std::make_unique<KeyboardActiveUnit>(director, converter, keys);
}


}  // namespace input
