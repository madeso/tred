#include "tred/input/keyboarddef.h"

#include <cassert>
#include <stdexcept>
#include <string>
#include "fmt/format.h"

#include "tred/input/config.h"
#include "tred/input/keyboardactiveunit.h"


namespace input
{

KeyboardDef::KeyboardDef(const config::KeyboardDef& data, ConverterDef* converter)
{
    for (const auto& d: data.binds)
    {
        keys.push_back(BindDef<Key>(d.bindname, d.key, d, converter));
    }
}


std::unique_ptr<ActiveUnit> KeyboardDef::Create(InputDirector* director, Converter* converter)
{
    assert(director);
    
    return std::make_unique<KeyboardActiveUnit>(director, converter, keys);
}


}  // namespace input
