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


struct KeyboardActiveUnit;
struct MouseActiveUnit;
struct JoystickActiveUnit;
struct GamecontrollerActiveUnit;


struct Director
{
    void add(KeyboardActiveUnit* kb);
    void remove(KeyboardActiveUnit* kb);

    void add(MouseActiveUnit* au);
    void remove(MouseActiveUnit* au);

    void add(JoystickActiveUnit* au);
    void remove(JoystickActiveUnit* au);

    void add(GamecontrollerActiveUnit* au);
    void remove(GamecontrollerActiveUnit* au);

    void on_keyboard_key(KeyboardKey key, bool down);

    void on_mouse_axis(Axis2 axis, float relative_state, float absolute_state);
    void on_mouse_wheel(Axis2 axis, float value);
    void on_mouse_button(MouseButton button, bool down);

    void on_joystick_ball(JoystickId joystick, Axis2 type, int ball, float value);
    void on_joystick_hat(JoystickId joystick, Axis2 type, int hat, float value);
    void on_joystick_button(JoystickId joystick, int button, bool down);
    void on_joystick_axis(JoystickId joystick, int axis, float value);
    void on_joystick_lost(JoystickId joystick);

    void on_gamecontroller_button(JoystickId joystick, GamecontrollerButton button, float state);
    void on_gamecontroller_axis(JoystickId joystick, GamecontrollerAxis axis, float value);
    void on_gamecontroller_lost(JoystickId joystick);

    bool was_just_pressed(KeyboardKey key);
    bool was_just_pressed(MouseButton button);
    bool was_just_pressed(JoystickId joy, int button);
    bool was_game_controller_start_just_pressed(JoystickId joy);
    // bool h(JoystickId joy);
    void remove_just_pressed();

    std::set<KeyboardKey> just_pressed_keys;
    std::set<MouseButton> just_pressed_mouse_buttons;
    std::set<SortablePair<JoystickId, int>> just_pressed_buttons;
    std::set<JoystickId> just_pressed_gamecontroller_starts;

    std::vector<KeyboardActiveUnit*> keyboards;
    std::vector<MouseActiveUnit*> mouses;
    std::map<JoystickId, JoystickActiveUnit*> joysticks;
    std::map<JoystickId, GamecontrollerActiveUnit*> gamecontrollers;
};

}
