#pragma once

#include <vector>
#include <memory>

#include "tred/input/bind.h"
#include "tred/input/unitdiscovery.h"


namespace input::config
{
    struct Mapping;
}


namespace input
{


struct UnitDef;
struct Director;
struct ActionMap;
struct ConnectedUnits;
struct UnitSetup;
struct Platform;


/** Contains a list of configurations.
    A good example is Mouse+Keyboard. The list could also be one gamepad.
    */
struct Mapping
{
    Mapping(const ActionMap& map, const config::Mapping& root);
    ~Mapping();

    void add(std::unique_ptr<UnitDef>&& unit);
    void add(std::unique_ptr<BindDefinition>&& bind);

    bool is_any_considered_joystick();
    bool can_detect(Director* director, unit_discovery discovery, UnitSetup* setup, Platform* platform);
    std::unique_ptr<ConnectedUnits> connect(Director* director, const UnitSetup& setup);

    std::vector<std::unique_ptr<UnitDef>> units;
    std::vector<std::unique_ptr<BindDefinition>> binds;
};


}
