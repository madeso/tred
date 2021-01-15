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
struct InputDirector;
struct InputActionMap;
struct ConnectedUnits;
struct UnitSetup;
struct Platform;


/** Contains a list of configurations.
    A good example is Mouse+Keyboard. The list could also be one gamepad.
    */
struct Mapping
{
    Mapping(const InputActionMap& map, const config::Mapping& root);
    ~Mapping();

    void Add(std::unique_ptr<UnitDef>&& unit);
    void Add(std::unique_ptr<BindDef>&& bind);

    bool IsAnyConsideredJoystick();
    bool CanDetect(InputDirector* director, UnitDiscovery discovery, UnitSetup* setup, Platform* platform);
    std::unique_ptr<ConnectedUnits> Connect(InputDirector* director, const UnitSetup& setup);

    std::vector<std::unique_ptr<UnitDef>> units;
    std::vector<std::unique_ptr<BindDef>> binds;
};


}  // namespace input
