#pragma once

#include <vector>
#include <memory>

#include "tred/input/bind.h"


namespace input::config
{
    struct Mapping;
}


namespace input
{


struct UnitDef;
struct InputDirector;
struct InputActionMap;
struct ConnectedUnits;
struct UnitSetup;


/** Contains a list of configurations.
    A good example is Mouse+Keyboard. The list could also be one gamepad.
    */
struct Mapping
{
    Mapping(const InputActionMap& map, const config::Mapping& root);
    ~Mapping();

    void Add(std::unique_ptr<UnitDef>&& def);

    std::unique_ptr<ConnectedUnits> Connect(InputDirector* director, const UnitSetup& setup);

    ConverterDef converter;
    std::vector<std::unique_ptr<UnitDef>> definitions;
};


}  // namespace input
