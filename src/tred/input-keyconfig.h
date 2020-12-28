#pragma once

#include <vector>
#include <memory>

#include "tred/input-connectedunits.h"
#include "tred/input-bind.h"


namespace input
{

struct UnitDef;
struct InputDirector;
struct InputActionMap;

/** Contains a list of configurations.
    A good example is Mouse+Keyboard.
    */
struct KeyConfig
{
    explicit KeyConfig(const InputActionMap& map);

    void Add(std::shared_ptr<UnitDef> def);

    std::shared_ptr<ConnectedUnits> Connect(InputDirector* director);

    Converter converter;
    std::vector<std::shared_ptr<UnitDef>> definitions;
};

}  // namespace input
