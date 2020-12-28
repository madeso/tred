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
    void Add(const std::string& name, std::shared_ptr<InputAction> action);

    std::shared_ptr<InputAction> Get(const std::string& name) const;

    std::vector<std::shared_ptr<InputAction>> GetActionList() const;

    std::map<std::string, std::shared_ptr<InputAction>> actions;
};


void Load(InputActionMap* map, const input::config::ActionMap& root);

}  // namespace input
