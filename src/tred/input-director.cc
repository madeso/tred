#include "tred/input-director.h"
#include <cassert>
#include <algorithm>

#include "tred/input-keyboardactiveunit.h"
#include "tred/input-mouseactiveunit.h"
#include "tred/input-joystickactiveunit.h"


namespace input
{
InputDirector::InputDirector()
{
}

InputDirector::~InputDirector()
{
}

void InputDirector::Add(KeyboardActiveUnit* kb)
{
    assert(kb);
    keyboards_.push_back(kb);
}

void InputDirector::Remove(KeyboardActiveUnit* kb)
{
    assert(kb);
    auto res = std::find(keyboards_.begin(), keyboards_.end(), kb);
    if (res != keyboards_.end())
    {
        /// @todo implement as a swap back and erase function
        keyboards_.erase(res);
    }
}

void InputDirector::Add(MouseActiveUnit* au)
{
    assert(au);
    mouse_.push_back(au);
}

void InputDirector::Remove(MouseActiveUnit* au)
{
    assert(au);
    auto res = std::find(mouse_.begin(), mouse_.end(), au);
    if (res != mouse_.end())
    {
        /// @todo implement as a swap back and erase function
        mouse_.erase(res);
    }
}

void InputDirector::Add(JoystickActiveUnit* au)
{
    assert(au);
    joystick_.push_back(au);
}

void InputDirector::Remove(JoystickActiveUnit* au)
{
    assert(au);
    auto res = std::find(joystick_.begin(), joystick_.end(), au);
    if (res != joystick_.end())
    {
        /// @todo implement as a swap back and erase function
        joystick_.erase(res);
    }
}

void InputDirector::OnKeyboardKey(Key key, bool down)
{
    for (auto kb: keyboards_)
    {
        kb->OnKey(key, down);
    }
}

void InputDirector::OnMouseAxis(Axis axis, float value)
{
    for (auto m: mouse_)
    {
        m->OnAxis(axis, value);
    }
}

void InputDirector::OnMouseButton(MouseButton key, bool down)
{
    for (auto m: mouse_)
    {
        m->OnButton(key, down ? 1.0f : 0.0f);
    }
}

void InputDirector::OnJoystickPov(Axis type, int hat, int, float value)
{
    /// @todo fix the joystick number

    for (auto j: joystick_)
    {
        j->OnHat(HatAxis(hat, type), value);
    }
}

void InputDirector::OnJoystickButton(int button, int, bool down)
{
    /// @todo fix the joystick number

    for (auto j: joystick_)
    {
        j->OnButton(button, down ? 1.0f : 0.0f);
    }
}

void InputDirector::OnJoystickAxis(int axis, int, float value)
{
    /// @todo fix the joystick number

    for (auto j: joystick_)
    {
        j->OnAxis(axis, value);
    }
}

}  // namespace input
