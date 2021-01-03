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

InputActionMap::~InputActionMap()
{
}

    
void InputActionMap::Add(const std::string& name, std::unique_ptr<InputAction>&& action)
{
    assert(action);
    actions.insert(std::make_pair(name, std::move(action)));
}


void Load(InputActionMap* map, const input::config::ActionMap& root)
{
    assert(map);

    for (const auto& d: root)
    {
        map->Add(d.name, std::make_unique<InputAction>(d.name, d.var, d.range));
    }
}


}  // namespace input