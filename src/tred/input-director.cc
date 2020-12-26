#include "tred/input-director.h"

#include <cassert>
#include <algorithm>

#include "tred/input-keyboardactiveunit.h"
#include "tred/input-mouseactiveunit.h"
#include "tred/input-joystickactiveunit.h"


namespace input
{


void InputDirector::Add(KeyboardActiveUnit* kb)
{
    assert(kb);
    keyboards.push_back(kb);
}


void InputDirector::Remove(KeyboardActiveUnit* kb)
{
    assert(kb);
    auto res = std::find(keyboards.begin(), keyboards.end(), kb);
    if (res != keyboards.end())
    {
        keyboards.erase(res);
    }
}


void InputDirector::Add(MouseActiveUnit* au)
{
    assert(au);
    mouses.push_back(au);
}


void InputDirector::Remove(MouseActiveUnit* au)
{
    assert(au);
    auto res = std::find(mouses.begin(), mouses.end(), au);
    if (res != mouses.end())
    {
        mouses.erase(res);
    }
}


void InputDirector::Add(JoystickActiveUnit* au)
{
    assert(au);
    joysticks.push_back(au);
}


void InputDirector::Remove(JoystickActiveUnit* au)
{
    assert(au);
    auto res = std::find(joysticks.begin(), joysticks.end(), au);
    if (res != joysticks.end())
    {
        /// @todo implement as a swap back and erase function
        joysticks.erase(res);
    }
}


void InputDirector::OnKeyboardKey(Key key, bool down)
{
    for (auto kb: keyboards)
    {
        kb->OnKey(key, down);
    }
}


void InputDirector::OnMouseAxis(Axis axis, float value)
{
    for (auto m: mouses)
    {
        m->OnAxis(axis, value);
    }
}


void InputDirector::OnMouseButton(MouseButton key, bool down)
{
    for (auto m: mouses)
    {
        m->OnButton(key, down ? 1.0f : 0.0f);
    }
}


void InputDirector::OnJoystickPov(Axis type, int hat, int, float value)
{
    /// @todo fix the joystick number

    for (auto j: joysticks)
    {
        j->OnHat(HatAxis(hat, type), value);
    }
}


void InputDirector::OnJoystickButton(int button, int, bool down)
{
    /// @todo fix the joystick number

    for (auto j: joysticks)
    {
        j->OnButton(button, down ? 1.0f : 0.0f);
    }
}


void InputDirector::OnJoystickAxis(int axis, int, float value)
{
    /// @todo fix the joystick number

    for (auto j: joysticks)
    {
        j->OnAxis(axis, value);
    }
}


}  // namespace input
