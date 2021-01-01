#pragma once

#include <vector>
#include <string>
#include <optional>

#include "tred/input/range.h"
#include "tred/input/axis.h"
#include "tred/input/key.h"

namespace input::config
{
struct Action
{
    Action(const std::string& name, const std::string& var, input::Range);
    std::string name;
    std::string var;
    input::Range range;
};

struct KeyboardButton
{
    KeyboardButton(const std::string&, input::Key, bool invert=false, float scale=1.0f);

    std::string bindname;

    input::Key key;

    bool invert;
    float scale;
};

struct MouseAxis
{
    MouseAxis(const std::string&, input::Axis, bool invert=false, float scale=1.0f);

    std::string bindname;

    input::Axis axis;

    bool invert;
    float scale;
};

struct MouseButton
{
    MouseButton(const std::string&, input::MouseButton, bool invert=false, float scale=1.0f);
    std::string bindname;

    input::MouseButton key;

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
    JoystickHat(const std::string&, int, input::Axis, bool invert=false, float scale=1.0f);

    std::string bindname;

    int hat;
    input::Axis axis;

    bool invert;
    float scale;
};

///////////////////////////////////////////////////////////////////////////////

using TwoButtonConverter = std::string;

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


struct Mapping
{
    Mapping(const std::string&, const std::vector<TwoButtonConverter>&, const std::vector<KeyboardDef>&, const std::vector<MouseDef>&, const std::vector<JoystickDef>&);

    std::string name;

    std::vector<TwoButtonConverter> two_button_converter;
    
    std::vector<KeyboardDef> keyboards;
    std::vector<MouseDef> mouses;
    std::vector<JoystickDef> joysticks;
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
