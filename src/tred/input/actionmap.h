#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>

#include "tred/result.h"
#include "tred/input/config.h"

namespace input
{


struct Table;
struct Action;


struct ActionMap
{
    ActionMap();
    ActionMap(ActionMap&& m);
    void operator=(ActionMap&& m);
    ~ActionMap();

    void add_action(const std::string& name, std::unique_ptr<Action>&& action);

    std::map<std::string, std::unique_ptr<Action>> actions;
};


Result<ActionMap> load_action_map(const input::config::ActionMap& root);

}  // namespace input
