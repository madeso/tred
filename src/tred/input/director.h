#pragma once

#include <vector>
#include <map>
#include <set>

#include "tred/input/key.h"
#include "tred/input/axis.h"
#include "tred/input/platform.h"
#include "tred/sortablepair.h"


namespace input
{


struct keyboard_active_unit;
struct mouse_active_unit;
struct joystick_active_unit;
struct gamecontroller_active_unit;


struct input_director
{
    void add(keyboard_active_unit* kb);
    void remove(keyboard_active_unit* kb);

    void add(mouse_active_unit* au);
    void remove(mouse_active_unit* au);

    void add(joystick_active_unit* au);
    void remove(joystick_active_unit* au);

    void add(gamecontroller_active_unit* au);
    void remove(gamecontroller_active_unit* au);

    void on_keyboard_key(keyboard_key key, bool down);

    void on_mouse_axis(xy_axis axis, float relative_state, float absolute_state);
    void on_mouse_wheel(xy_axis axis, float value);
    void on_mouse_button(mouse_button button, bool down);

    void on_joystick_ball(joystick_id joystick, xy_axis type, int ball, float value);
    void on_joystick_hat(joystick_id joystick, xy_axis type, int hat, float value);
    void on_joystick_button(joystick_id joystick, int button, bool down);
    void on_joystick_axis(joystick_id joystick, int axis, float value);
    void on_joystick_lost(joystick_id joystick);

    void on_gamecontroller_button(joystick_id joystick, gamecontroller_button button, float state);
    void on_gamecontroller_axis(joystick_id joystick, gamecontroller_axis axis, float value);
    void on_gamecontroller_lost(joystick_id joystick);

    bool was_just_pressed(keyboard_key key);
    bool was_just_pressed(mouse_button button);
    bool was_just_pressed(joystick_id joy, int button);
    bool was_game_controller_start_just_pressed(joystick_id joy);
    // bool h(JoystickId joy);
    void remove_just_pressed();

    std::set<keyboard_key> just_pressed_keys;
    std::set<mouse_button> just_pressed_mouse_buttons;
    std::set<sortable_pair<joystick_id, int>> just_pressed_buttons;
    std::set<joystick_id> just_pressed_gamecontroller_starts;

    std::vector<keyboard_active_unit*> keyboards;
    std::vector<mouse_active_unit*> mouses;
    std::map<joystick_id, joystick_active_unit*> joysticks;
    std::map<joystick_id, gamecontroller_active_unit*> gamecontrollers;
};

}
