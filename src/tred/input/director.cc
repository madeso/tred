#include "tred/input/director.h"

#include "tred/assert.h"
#include <algorithm>

#include "tred/input/activeunit.keyboard.h"
#include "tred/input/activeunit.mouse.h"
#include "tred/input/activeunit.joystick.h"
#include "tred/input/activeunit.gamecontroller.h"


namespace input
{


void Director::add(KeyboardActiveUnit* kb)
{
    ASSERT(kb);
    keyboards.push_back(kb);
}


void Director::remove(KeyboardActiveUnit* kb)
{
    ASSERT(kb);
    auto res = std::find(keyboards.begin(), keyboards.end(), kb);
    if (res != keyboards.end())
    {
        keyboards.erase(res);
    }
}


void Director::add(MouseActiveUnit* au)
{
    ASSERT(au);
    mouses.push_back(au);
}


void Director::remove(MouseActiveUnit* au)
{
    ASSERT(au);
    auto res = std::find(mouses.begin(), mouses.end(), au);
    if (res != mouses.end())
    {
        mouses.erase(res);
    }
}


void Director::add(JoystickActiveUnit* au)
{
    ASSERT(au);
    joysticks.insert({au->joystick, au});
}


void Director::remove(JoystickActiveUnit* au)
{
    ASSERT(au);
    joysticks.erase(au->joystick);
}


void Director::add(GamecontrollerActiveUnit* au)
{
    ASSERT(au);
    gamecontrollers.insert({au->joystick, au});
}


void Director::remove(GamecontrollerActiveUnit* au)
{
    ASSERT(au);
    gamecontrollers.erase(au->joystick);
}


///////////////////////////////////////////////////////////////////////////////


void Director::on_keyboard_key(KeyboardKey key, bool down)
{
    if(!down && was_just_pressed(key) == false)
    {
        just_pressed_keys.insert(key);
    }

    for (auto kb: keyboards)
    {
        kb->on_key(key, down);
    }
}


///////////////////////////////////////////////////////////////////////////////


void Director::on_mouse_axis(Axis2 axis, float relative_state, float absolute_state)
{
    for (auto m: mouses)
    {
        m->on_axis(axis, relative_state, absolute_state);
    }
}

void Director::on_mouse_wheel(Axis2 axis, float value)
{
    for (auto m: mouses)
    {
        m->on_wheel(axis, value);
    }
}


void Director::on_mouse_button(MouseButton key, bool down)
{
    if(!down && was_just_pressed(key) == false)
    {
        just_pressed_mouse_buttons.insert(key);
    }

    for (auto m: mouses)
    {
        m->on_button(key, down ? 1.0f : 0.0f);
    }
}


///////////////////////////////////////////////////////////////////////////////


void Director::on_joystick_ball(JoystickId joystick, Axis2 axis, int ball, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->on_ball({ball, axis}, value);
}


void Director::on_joystick_hat(JoystickId joystick, Axis2 axis, int hat, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->on_hat({hat, axis}, value);
}


void Director::on_joystick_button(JoystickId joystick, int button, bool down)
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

    found->second->on_button(button, down);
}


void Director::on_joystick_axis(JoystickId joystick, int axis, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->on_axis(axis, value);
}


void Director::on_joystick_lost(JoystickId joystick)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->scheduled_delete = true;
    joysticks.erase(joystick);
}


///////////////////////////////////////////////////////////////////////////////


void Director::on_gamecontroller_button(JoystickId joystick, GamecontrollerButton button, float state)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end())
    {
        if(state > 0.5f && button == GamecontrollerButton::start)
        {
            just_pressed_gamecontroller_starts.insert(joystick);
        }

        return;
    }

    found->second->on_button(button, state);
}


void Director::on_gamecontroller_axis(JoystickId joystick, GamecontrollerAxis axis, float value)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end()) { return; }

    found->second->on_axis(axis, value);
}


void Director::on_gamecontroller_lost(JoystickId joystick)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end()) { return; }

    found->second->scheduled_delete = true;
    gamecontrollers.erase(joystick);
}


///////////////////////////////////////////////////////////////////////////////


bool Director::was_just_pressed(KeyboardKey key)
{
    return just_pressed_keys.find(key) != just_pressed_keys.end();
}


bool Director::was_just_pressed(MouseButton button)
{
    return just_pressed_mouse_buttons.find(button) != just_pressed_mouse_buttons.end();
}


bool Director::was_just_pressed(JoystickId joy, int button)
{
    return just_pressed_buttons.find({joy, button}) != just_pressed_buttons.end();
}


bool Director::was_game_controller_start_just_pressed(JoystickId joy)
{
    return just_pressed_gamecontroller_starts.find(joy) != just_pressed_gamecontroller_starts.end();
}


void Director::remove_just_pressed()
{
    just_pressed_keys.clear();
    just_pressed_mouse_buttons.clear();
    just_pressed_buttons.clear();
    just_pressed_gamecontroller_starts.clear();
}


}  // namespace input
