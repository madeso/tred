#pragma once

#include <vector>

#include "tred/input/key.h"
#include "tred/input/axis.h"


namespace input
{


struct KeyboardActiveUnit;
struct MouseActiveUnit;
struct JoystickActiveUnit;


struct InputDirector
{
    void Add(KeyboardActiveUnit* kb);
    void Remove(KeyboardActiveUnit* kb);

    void Add(MouseActiveUnit* au);
    void Remove(MouseActiveUnit* au);

    void Add(JoystickActiveUnit* au);
    void Remove(JoystickActiveUnit* au);

    void OnKeyboardKey(Key key, bool down);

    void OnMouseAxis(Axis axis, float value);
    void OnMouseButton(MouseButton button, bool down);

    void OnJoystickPov(Axis type, int hat, int joystick, float value);
    void OnJoystickButton(int button, int joystick, bool down);
    void OnJoystickAxis(int axis, int joystick, float value);

    std::vector<KeyboardActiveUnit*> keyboards;
    std::vector<MouseActiveUnit*> mouses;
    std::vector<JoystickActiveUnit*> joysticks;
};

}  // namespace input
