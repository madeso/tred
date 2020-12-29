#pragma once

#include <map>
#include <string>
#include <memory>
#include <vector>


#include "tred/input-config.h"

namespace input
{


struct Table;
struct InputAction;


struct InputActionMap
{
    InputActionMap();
    ~InputActionMap();
    void Add(const std::string& name, std::unique_ptr<InputAction>&& action);

    std::map<std::string, std::unique_ptr<InputAction>> actions;
};


void Load(InputActionMap* map, const input::config::ActionMap& root);

}  // namespace input
