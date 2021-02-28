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


struct InputDirector
{
    void Add(KeyboardActiveUnit* kb);
    void Remove(KeyboardActiveUnit* kb);

    void Add(MouseActiveUnit* au);
    void Remove(MouseActiveUnit* au);

    void Add(JoystickActiveUnit* au);
    void Remove(JoystickActiveUnit* au);

    void Add(GamecontrollerActiveUnit* au);
    void Remove(GamecontrollerActiveUnit* au);

    void OnKeyboardKey(Key key, bool down);

    void OnMouseAxis(Axis axis, float relative_state, float absolute_state);
    void OnMouseWheel(Axis axis, float value);
    void OnMouseButton(MouseButton button, bool down);

    void OnJoystickBall(JoystickId joystick, Axis type, int ball, float value);
    void OnJoystickHat(JoystickId joystick, Axis type, int hat, float value);
    void OnJoystickButton(JoystickId joystick, int button, bool down);
    void OnJoystickAxis(JoystickId joystick, int axis, float value);
    void OnJoystickLost(JoystickId joystick);

    void OnGamecontrollerButton(JoystickId joystick, GamecontrollerButton button, float state);
    void OnGamecontrollerAxis(JoystickId joystick, GamecontrollerAxis axis, float value);
    void OnGamecontrollerLost(JoystickId joystick);

    bool WasJustPressed(Key key);
    bool WasJustPressed(MouseButton button);
    bool WasJustPressed(JoystickId joy, int button);
    bool WasGameControllerStartJustPressed(JoystickId joy);
    bool h(JoystickId joy);
    void RemoveJustPressed();

    std::set<Key> just_pressed_keys;
    std::set<MouseButton> just_pressed_mouse_buttons;
    std::set<SortablePair<JoystickId, int>> just_pressed_buttons;
    std::set<JoystickId> just_pressed_gamecontroller_starts;

    std::vector<KeyboardActiveUnit*> keyboards;
    std::vector<MouseActiveUnit*> mouses;
    std::map<JoystickId, JoystickActiveUnit*> joysticks;
    std::map<JoystickId, GamecontrollerActiveUnit*> gamecontrollers;
};

}  // namespace input
