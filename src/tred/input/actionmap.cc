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


input_action_map::input_action_map()
{
}


input_action_map::input_action_map(input_action_map&& m)
    : actions(std::move(m.actions))
{
}


void input_action_map::operator=(input_action_map&& m)
{
    actions = std::move(m.actions);
}


input_action_map::~input_action_map()
{
}


void input_action_map::add_action(const std::string& name, std::unique_ptr<input_action>&& action)
{
    assert(action);
    actions.insert(std::make_pair(name, std::move(action)));
}


result<input_action_map> load_action_map(const input::config::action_map& root)
{
    input_action_map map;

    for (const auto& d: root)
    {
        const auto existing = map.actions.find(d.name) != map.actions.end();
        if(existing)
        {
            return fmt::format("Duplicate action found {}", d.name);
        }
        map.add_action(d.name, std::make_unique<input_action>(d.name, d.var, d.range));
    }

    return std::move(map);
}


}  // namespace input
