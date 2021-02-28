#include "tred/input/director.h"

#include <cassert>
#include <algorithm>

#include "tred/input/activeunit.keyboard.h"
#include "tred/input/activeunit.mouse.h"
#include "tred/input/activeunit.joystick.h"
#include "tred/input/activeunit.gamecontroller.h"


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


void InputDirector::Add(GamecontrollerActiveUnit* au)
{
    assert(au);
    gamecontrollers.insert({au->joystick, au});
}


void InputDirector::Remove(GamecontrollerActiveUnit* au)
{
    assert(au);
    gamecontrollers.erase(au->joystick);
}


///////////////////////////////////////////////////////////////////////////////


void InputDirector::OnKeyboardKey(Key key, bool down)
{
    if(!down && WasJustPressed(key) == false)
    {
        just_pressed_keys.insert(key);
    }

    for (auto kb: keyboards)
    {
        kb->OnKey(key, down);
    }
}


///////////////////////////////////////////////////////////////////////////////


void InputDirector::OnMouseAxis(Axis axis, float relative_state, float absolute_state)
{
    for (auto m: mouses)
    {
        m->OnAxis(axis, relative_state, absolute_state);
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
    if(!down && WasJustPressed(key) == false)
    {
        just_pressed_mouse_buttons.insert(key);
    }

    for (auto m: mouses)
    {
        m->OnButton(key, down ? 1.0f : 0.0f);
    }
}


///////////////////////////////////////////////////////////////////////////////


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
    if(found == joysticks.end())
    {
        if(down == false)
        {
            just_pressed_buttons.insert({joystick, button});
        }

        return;
    }

    found->second->OnButton(button, down);
}


void InputDirector::OnJoystickAxis(JoystickId joystick, int axis, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->OnAxis(axis, value);
}


void InputDirector::OnJoystickLost(JoystickId joystick)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->sheduled_delete = true;
    joysticks.erase(joystick);
}


///////////////////////////////////////////////////////////////////////////////


void InputDirector::OnGamecontrollerButton(JoystickId joystick, GamecontrollerButton button, float state)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end())
    {
        if(state > 0.5f && button == GamecontrollerButton::START)
        {
            just_pressed_gamecontroller_starts.insert(joystick);
        }

        return;
    }

    found->second->OnButton(button, state);
}


void InputDirector::OnGamecontrollerAxis(JoystickId joystick, GamecontrollerAxis axis, float value)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end()) { return; }

    found->second->OnAxis(axis, value);
}


void InputDirector::OnGamecontrollerLost(JoystickId joystick)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end()) { return; }

    found->second->sheduled_delete = true;
    gamecontrollers.erase(joystick);
}


///////////////////////////////////////////////////////////////////////////////


bool InputDirector::WasJustPressed(Key key)
{
    return just_pressed_keys.find(key) != just_pressed_keys.end();
}


bool InputDirector::WasJustPressed(MouseButton button)
{
    return just_pressed_mouse_buttons.find(button) != just_pressed_mouse_buttons.end();
}


bool InputDirector::WasJustPressed(JoystickId joy, int button)
{
    return just_pressed_buttons.find({joy, button}) != just_pressed_buttons.end();
}


bool InputDirector::WasGameControllerStartJustPressed(JoystickId joy)
{
    return just_pressed_gamecontroller_starts.find(joy) != just_pressed_gamecontroller_starts.end();
}


void InputDirector::RemoveJustPressed()
{
    just_pressed_keys.clear();
    just_pressed_mouse_buttons.clear();
    just_pressed_buttons.clear();
    just_pressed_gamecontroller_starts.clear();
}


}  // namespace input
