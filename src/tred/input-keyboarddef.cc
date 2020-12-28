#include "tred/input-keyboarddef.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include "fmt/format.h"

#include "tred/input-config.h"
#include "tred/input-keyboardactiveunit.h"


namespace input
{

KeyboardDef::KeyboardDef(const config::KeyboardDef& data, Converter* converter)
{
    for (const auto& d: data.binds)
    {
        keys.push_back(BindDef<Key>(d.bindname, d.key, d, converter));
    }
}


std::shared_ptr<ActiveUnit> KeyboardDef::Create(InputDirector* director)
{
    assert(director);

    std::shared_ptr<ActiveUnit> unit(new KeyboardActiveUnit(director, keys));
    
    return unit;
}


}  // namespace input
