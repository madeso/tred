#pragma once



#include "tred/input/unitdef.h"
#include "tred/input/bind.h"
#include "tred/input/hataxis.h"


namespace input::config
{
struct GamecontrollerDefinition;
}


namespace input
{

struct Director;
struct ActiveUnit;
struct ActionMap;
struct Platform;


struct GamecontrollerUnitDef : public UnitDef
{
    GamecontrollerUnitDef(int index, const config::GamecontrollerDefinition& data);

    std::optional<std::string> validate_key(int key) override;
    std::optional<std::string> validate_axis(AxisType type, int target, int axis) override;

    bool is_considered_joystick() override;
    bool can_detect(Director* director, unit_discovery discovery, UnitSetup* setup, Platform* platform) override;
    std::unique_ptr<ActiveUnit> create(Director* director, const UnitSetup& setup) override;

    int index;
};


}  // namespace input
