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
struct InputAction;


struct InputActionMap
{
    InputActionMap();
    InputActionMap(InputActionMap&& m);
    void operator=(InputActionMap&& m);
    ~InputActionMap();

    void Add(const std::string& name, std::unique_ptr<InputAction>&& action);

    std::map<std::string, std::unique_ptr<InputAction>> actions;
};


Result<InputActionMap> LoadActionMap(const input::config::ActionMap& root);

}  // namespace input
