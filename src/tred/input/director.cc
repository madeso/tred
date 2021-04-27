#include "tred/input/director.h"

#include <cassert>
#include <algorithm>

#include "tred/input/activeunit.keyboard.h"
#include "tred/input/activeunit.mouse.h"
#include "tred/input/activeunit.joystick.h"
#include "tred/input/activeunit.gamecontroller.h"


namespace input
{


void input_director::add(keyboard_active_unit* kb)
{
    assert(kb);
    keyboards.push_back(kb);
}


void input_director::remove(keyboard_active_unit* kb)
{
    assert(kb);
    auto res = std::find(keyboards.begin(), keyboards.end(), kb);
    if (res != keyboards.end())
    {
        keyboards.erase(res);
    }
}


void input_director::add(mouse_active_unit* au)
{
    assert(au);
    mouses.push_back(au);
}


void input_director::remove(mouse_active_unit* au)
{
    assert(au);
    auto res = std::find(mouses.begin(), mouses.end(), au);
    if (res != mouses.end())
    {
        mouses.erase(res);
    }
}


void input_director::add(joystick_active_unit* au)
{
    assert(au);
    joysticks.insert({au->joystick, au});
}


void input_director::remove(joystick_active_unit* au)
{
    assert(au);
    joysticks.erase(au->joystick);
}


void input_director::add(gamecontroller_active_unit* au)
{
    assert(au);
    gamecontrollers.insert({au->joystick, au});
}


void input_director::remove(gamecontroller_active_unit* au)
{
    assert(au);
    gamecontrollers.erase(au->joystick);
}


///////////////////////////////////////////////////////////////////////////////


void input_director::on_keyboard_key(keyboard_key key, bool down)
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


void input_director::on_mouse_axis(xy_axis axis, float relative_state, float absolute_state)
{
    for (auto m: mouses)
    {
        m->on_axis(axis, relative_state, absolute_state);
    }
}

void input_director::on_mouse_wheel(xy_axis axis, float value)
{
    for (auto m: mouses)
    {
        m->on_wheel(axis, value);
    }
}


void input_director::on_mouse_button(mouse_button key, bool down)
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


void input_director::on_joystick_ball(joystick_id joystick, xy_axis axis, int ball, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->on_ball({ball, axis}, value);
}


void input_director::on_joystick_hat(joystick_id joystick, xy_axis axis, int hat, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->on_hat({hat, axis}, value);
}


void input_director::on_joystick_button(joystick_id joystick, int button, bool down)
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


void input_director::on_joystick_axis(joystick_id joystick, int axis, float value)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->on_axis(axis, value);
}


void input_director::on_joystick_lost(joystick_id joystick)
{
    auto found = joysticks.find(joystick);
    if(found == joysticks.end()) { return; }

    found->second->scheduled_delete = true;
    joysticks.erase(joystick);
}


///////////////////////////////////////////////////////////////////////////////


void input_director::on_gamecontroller_button(joystick_id joystick, gamecontroller_button button, float state)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end())
    {
        if(state > 0.5f && button == gamecontroller_button::start)
        {
            just_pressed_gamecontroller_starts.insert(joystick);
        }

        return;
    }

    found->second->on_button(button, state);
}


void input_director::on_gamecontroller_axis(joystick_id joystick, gamecontroller_axis axis, float value)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end()) { return; }

    found->second->on_axis(axis, value);
}


void input_director::on_gamecontroller_lost(joystick_id joystick)
{
    auto found = gamecontrollers.find(joystick);
    if(found == gamecontrollers.end()) { return; }

    found->second->scheduled_delete = true;
    gamecontrollers.erase(joystick);
}


///////////////////////////////////////////////////////////////////////////////


bool input_director::was_just_pressed(keyboard_key key)
{
    return just_pressed_keys.find(key) != just_pressed_keys.end();
}


bool input_director::was_just_pressed(mouse_button button)
{
    return just_pressed_mouse_buttons.find(button) != just_pressed_mouse_buttons.end();
}


bool input_director::was_just_pressed(joystick_id joy, int button)
{
    return just_pressed_buttons.find({joy, button}) != just_pressed_buttons.end();
}


bool input_director::was_game_controller_start_just_pressed(joystick_id joy)
{
    return just_pressed_gamecontroller_starts.find(joy) != just_pressed_gamecontroller_starts.end();
}


void input_director::remove_just_pressed()
{
    just_pressed_keys.clear();
    just_pressed_mouse_buttons.clear();
    just_pressed_buttons.clear();
    just_pressed_gamecontroller_starts.clear();
}


}  // namespace input
