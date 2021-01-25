#include "tred/input/actionmap.h"
#include <cassert>
#include <stdexcept>
#include <fstream>
#include "fmt/format.h"

#include "tred/input/config.h"
#include "tred/input/range.h"
#include "tred/input/action.h"


namespace input
{


InputActionMap::InputActionMap()
{
}


InputActionMap::InputActionMap(InputActionMap&& m)
    : actions(std::move(m.actions))
{
}


void InputActionMap::operator=(InputActionMap&& m)
{
    actions = std::move(m.actions);
}


InputActionMap::~InputActionMap()
{
}


void InputActionMap::Add(const std::string& name, std::unique_ptr<InputAction>&& action)
{
    assert(action);
    actions.insert(std::make_pair(name, std::move(action)));
}


Result<InputActionMap> LoadActionMap(const input::config::ActionMap& root)
{
    InputActionMap map;

    for (const auto& d: root)
    {
        const auto existing = map.actions.find(d.name) != map.actions.end();
        if(existing)
        {
            return fmt::format("Duplicate action found {}", d.name);
        }
        map.Add(d.name, std::make_unique<InputAction>(d.name, d.var, d.range));
    }

    return std::move(map);
}


}  // namespace input
