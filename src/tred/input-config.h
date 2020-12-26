#pragma once

#include <vector>
#include <string>
#include <optional>

#include "tred/input-range.h"
#include "tred/input-axis.h"
#include "tred/input-key.h"

namespace input::config
{
struct Action
{
    Action(const std::string& name, const std::string& var, ::Range, bool global=false);
    std::string name;
    std::string var;
    ::Range range;
    bool global;
};

struct KeyboardButton
{
    KeyboardButton(const std::string&, ::Key, bool invert=false, float scale=1.0f);

    std::string bindname;

    ::Key key;

    bool invert;
    float scale;
};

struct MouseAxis
{
    MouseAxis(const std::string&, ::Axis, bool invert=false, float scale=1.0f);

    std::string bindname;

    ::Axis axis;

    bool invert;
    float scale;
};

struct MouseButton
{
    MouseButton(const std::string&, ::MouseButton, bool invert=false, float scale=1.0f);
    std::string bindname;

    ::MouseButton key;

    bool invert;
    float scale;
};


struct JoystickAxis
{
    JoystickAxis(const std::string&, int, bool invert=false, float scale=1.0f);

    std::string bindname;

    int axis;

    bool invert;
    float scale;
};

struct JoystickButton
{
    JoystickButton(const std::string&, int, bool invert=false, float scale=1.0f);
    std::string bindname;

    int button;

    bool invert;
    float scale;
};

struct JoystickHat
{
    JoystickHat(const std::string&, int, ::Axis, bool invert=false, float scale=1.0f);

    std::string bindname;

    int hat;
    ::Axis axis;

    bool invert;
    float scale;
};

///////////////////////////////////////////////////////////////////////////////

struct KeyboardDef
{
    KeyboardDef();
    KeyboardDef(const std::vector<KeyboardButton>&);

    std::vector<KeyboardButton> binds;
};

struct MouseDef
{
    MouseDef();
    MouseDef(const std::vector<MouseAxis>&, const std::vector<MouseButton>&);

    std::vector<MouseAxis> axis;
    std::vector<MouseButton> button;
};

struct JoystickDef
{
    JoystickDef();
    JoystickDef(const std::vector<JoystickAxis>&, const std::vector<JoystickButton>&, const std::vector<JoystickHat>&);

    std::vector<JoystickAxis> axis;
    std::vector<JoystickButton> button;
    std::vector<JoystickHat> hat;
};


// todo(Gustav): move one layer up
struct UnitInterface
{
    UnitInterface(const KeyboardDef&);
    UnitInterface(const MouseDef&);
    UnitInterface(const JoystickDef&);

    std::optional<KeyboardDef> keyboard;
    std::optional<MouseDef> mouse;
    std::optional<JoystickDef> joystick;
};

struct Config
{
    Config(const std::string&, const std::vector<UnitInterface>&);

    std::string name;
    std::vector<UnitInterface> units;
};

///////////////////////////////////////////////////////////////////////////////
// roots

using ActionMap = std::vector<Action>;
using KeyConfigs = std::vector<Config>;

struct InputSystem
{
    InputSystem(const ActionMap&, const KeyConfigs&, const std::vector<std::string>&);

    ActionMap actions;
    KeyConfigs keys;
    std::vector<std::string> players;
};

}
