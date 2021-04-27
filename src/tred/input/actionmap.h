#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>

#include "tred/result.h"
#include "tred/input/config.h"

namespace input
{


struct table;
struct input_action;


struct input_action_map
{
    input_action_map();
    input_action_map(input_action_map&& m);
    void operator=(input_action_map&& m);
    ~input_action_map();

    void add_action(const std::string& name, std::unique_ptr<input_action>&& action);

    std::map<std::string, std::unique_ptr<input_action>> actions;
};


result<input_action_map> load_action_map(const input::config::action_map& root);

}  // namespace input
