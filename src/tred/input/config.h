#pragma once

#include <vector>
#include <string>
#include <optional>
#include <string_view>

#include "tred/input/range.h"
#include "tred/input/axis.h"
#include "tred/input/key.h"
#include "tred/input/axistype.h"


namespace input::config
{


struct Action
{
    Action(const std::string& name, const std::string& var, input::Range);
    Action(const std::string& name_and_var, input::Range);
    Action(const std::string_view& name_and_var, input::Range);

    std::string name;
    std::string var;
    input::Range range;
};


struct KeyboardDef
{
    KeyboardDef();
    KeyboardDef(Key);

    Key detection_key;
};


struct MouseDef
{
    MouseDef();
    MouseDef(input::MouseButton);

    input::MouseButton detection_button;
};


struct JoystickDef
{
    JoystickDef();
    JoystickDef(int, const std::string&);

    int start_button;
    std::string unit;
};


struct UnitDef
{
    UnitDef(const KeyboardDef&);
    UnitDef(const MouseDef&);
    UnitDef(const JoystickDef&);

    std::optional<KeyboardDef> keyboard;
    std::optional<MouseDef> mouse;
    std::optional<JoystickDef> joystick;
};


struct KeyBindDef
{
    KeyBindDef(const std::string&, int, input::Key);
    KeyBindDef(const std::string&, int, input::MouseButton);
    KeyBindDef(const std::string&, int, int);

    std::string action;
    int unit;
    int key;
};


struct AxisBindDef
{
    AxisBindDef(const std::string&, int, input::Axis, float sens = 1.0f, bool ii = false);
    AxisBindDef(const std::string&, int, input::AxisType, int, int, float sens=1.0f, bool ii = false);

    std::string action;
    int unit;
    AxisType type;
    int target; // (joystick axis, hat or ball index)
    int axis;
    float sensitivity;
    bool is_inverted;
};


struct TwoKeyBindDef
{
    TwoKeyBindDef(const std::string&, int, input::Key, input::Key);
    TwoKeyBindDef(const std::string&, int, int, int);

    std::string action;
    int unit;
    int negative;
    int positive;
};


struct BindDef
{
    BindDef(const KeyBindDef&);
    BindDef(const AxisBindDef&);
    BindDef(const TwoKeyBindDef&);

    std::optional<KeyBindDef> key;
    std::optional<AxisBindDef> axis;
    std::optional<TwoKeyBindDef> twokey;
};


using UnitDefs = std::vector<UnitDef>;
using BindDefs = std::vector<BindDef>;


struct Mapping
{
    Mapping(const std::string&, const std::vector<UnitDef>&, const std::vector<BindDef>&);

    std::string name;

    std::vector<UnitDef> units;
    std::vector<BindDef> binds;
};

///////////////////////////////////////////////////////////////////////////////
// roots

using ActionMap = std::vector<Action>;
using MappingList = std::vector<Mapping>;

struct InputSystem
{
    InputSystem(const ActionMap&, const MappingList&);

    ActionMap actions;
    MappingList keys;
};

}
