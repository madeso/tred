#pragma once

#include <vector>
#include <memory>

#include "tred/input-connectedunits.h"


namespace input
{

struct UnitDef;
struct InputDirector;
struct InputActionMap;
struct BindMap;

/** Contains a list of configurations.
    A good example is Mouse+Keyboard.
    */
struct KeyConfig
{
    KeyConfig();

    void Add(std::shared_ptr<UnitDef> def);
    std::shared_ptr<ConnectedUnits> Connect(const InputActionMap& actions, InputDirector* director);

    std::vector<std::shared_ptr<UnitDef>> definitions;
    std::shared_ptr<BindMap> binds;
};

}  // namespace input
