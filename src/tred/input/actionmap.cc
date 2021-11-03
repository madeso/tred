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


ActionMap::ActionMap()
{
}


ActionMap::ActionMap(ActionMap&& m)
    : actions(std::move(m.actions))
{
}


void ActionMap::operator=(ActionMap&& m)
{
    actions = std::move(m.actions);
}


ActionMap::~ActionMap()
{
}


void ActionMap::add_action(const std::string& name, std::unique_ptr<Action>&& action)
{
    assert(action);
    actions.insert(std::make_pair(name, std::move(action)));
}


Result<ActionMap> load_action_map(const input::config::ActionMap& root)
{
    ActionMap map;

    for (const auto& d: root)
    {
        const auto existing = map.actions.find(d.name) != map.actions.end();
        if(existing)
        {
            return fmt::format("Duplicate action found {}", d.name);
        }
        map.add_action(d.name, std::make_unique<Action>(d.name, d.var, d.range));
    }

    return map;
}


}  // namespace input
