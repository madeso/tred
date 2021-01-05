#include "tred/input/director.h"

#include <cassert>
#include <algorithm>

#include "tred/input/keyboardactiveunit.h"
#include "tred/input/mouseactiveunit.h"
#include "tred/input/joystickactiveunit.h"


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
    joysticks.insert({au->joystick, au});
}


void InputDirector::Remove(JoystickActiveUnit* au)
{
    assert(au);
    joysticks.erase(au->joystick);
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

void InputDirector::OnMouseWheel(Axis axis, float value)
{
    for (auto m: mouses)
    {
        m->OnWheel(axis, value);
    }
}


void InputDirector::OnMouseButton(MouseButton key, bool down)
{
    for (auto m: mouses)
    {
        m->OnButton(key, down ? 1.0f : 0.0f);
    }
}


void InputDirector::OnJoystickBall(JoystickId joystick, Axis axis, int ball, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->OnBall({ball, axis}, value);
}


void InputDirector::OnJoystickHat(JoystickId joystick, Axis axis, int hat, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->OnHat({hat, axis}, value);
}


void InputDirector::OnJoystickButton(JoystickId joystick, int button, bool down)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->OnButton(button, down);
}


void InputDirector::OnJoystickAxis(JoystickId joystick, int axis, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->OnAxis(axis, value);
}




}  // namespace input
