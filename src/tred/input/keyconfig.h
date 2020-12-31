#pragma once

#include <vector>
#include <memory>

#include "tred/input/bind.h"


namespace input
{


struct UnitDef;
struct InputDirector;
struct InputActionMap;
struct ConnectedUnits;


/** Contains a list of configurations.
    A good example is Mouse+Keyboard.
    */
struct KeyConfig
{
    explicit KeyConfig(const InputActionMap& map);
    ~KeyConfig();

    void Add(std::unique_ptr<UnitDef>&& def);

    std::unique_ptr<ConnectedUnits> Connect(InputDirector* director);

    ConverterDef converter;
    std::vector<std::unique_ptr<UnitDef>> definitions;
};


}  // namespace input
