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


struct action
{
    action(const std::string& name, const std::string& var, input::range);
    action(const std::string& name_and_var, input::range);
    action(const std::string_view& name_and_var, input::range);

    std::string name;
    std::string var;
    input::range range;
};


struct keyboard_definition
{
    keyboard_definition();
    keyboard_definition(keyboard_key);

    keyboard_key detection_key;
};


struct mouse_definition
{
    mouse_definition();
    mouse_definition(input::mouse_button);

    input::mouse_button detection_button;
};


struct joystick_definition
{
    joystick_definition();
    joystick_definition(int, const std::string&);

    int start_button;
    std::string unit;
};


struct gamecontroller_definition
{
};


struct unit_definition
{
    unit_definition(const keyboard_definition&);
    unit_definition(const mouse_definition&);
    unit_definition(const joystick_definition&);
    unit_definition(const gamecontroller_definition&);

    std::optional<keyboard_definition> keyboard;
    std::optional<mouse_definition> mouse;
    std::optional<joystick_definition> joystick;
    std::optional<gamecontroller_definition> gamecontroller;
};


struct key_bind_definition
{
    key_bind_definition(const std::string&, int, input::keyboard_key);
    key_bind_definition(const std::string&, int, input::gamecontroller_button);
    key_bind_definition(const std::string&, int, input::mouse_button);
    key_bind_definition(const std::string&, int, int);

    std::string action;
    int unit;
    int key;
};


struct axis_bind_definition
{
    axis_bind_definition(const std::string&, int, input::xy_axis, float sens = 1.0f, bool ii = false);
    axis_bind_definition(const std::string&, int, input::gamecontroller_axis, float sens = 1.0f, bool ii = false);
    axis_bind_definition(const std::string&, int, input::axis_type, int, int, float sens=1.0f, bool ii = false);

    std::string action;
    int unit;
    axis_type type;
    int target; // (joystick axis, hat or ball index)
    int axis;
    float sensitivity;
    bool is_inverted;
};


struct two_key_bind_definition
{
    two_key_bind_definition(const std::string&, int, input::keyboard_key, input::keyboard_key);
    two_key_bind_definition(const std::string&, int, input::gamecontroller_button, input::gamecontroller_button);
    two_key_bind_definition(const std::string&, int, int, int);

    std::string action;
    int unit;
    int negative;
    int positive;
};


struct bind_definition
{
    bind_definition(const key_bind_definition&);
    bind_definition(const axis_bind_definition&);
    bind_definition(const two_key_bind_definition&);

    std::optional<key_bind_definition> key;
    std::optional<axis_bind_definition> axis;
    std::optional<two_key_bind_definition> twokey;
};


using unit_definitions = std::vector<unit_definition>;
using bind_definitions = std::vector<bind_definition>;


struct mapping
{
    mapping(const std::string&, const std::vector<unit_definition>&, const std::vector<bind_definition>&);

    std::string name;

    std::vector<unit_definition> units;
    std::vector<bind_definition> binds;
};

///////////////////////////////////////////////////////////////////////////////
// roots

using action_map = std::vector<action>;
using mapping_list = std::vector<mapping>;

struct input_system
{
    input_system(const action_map&, const mapping_list&);

    action_map actions;
    mapping_list keys;
};

}
