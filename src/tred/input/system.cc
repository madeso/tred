#include "tred/input/system.h"

#include <cassert>
#include <fstream>

#include "fmt/format.h"

#include "tred/input/config.h"
#include "tred/input/director.h"
#include "tred/input/player.h"
#include "tred/input/mapping.h"
#include "tred/input/connectedunits.h"
#include "tred/input/mappinglist.h"
#include "tred/input/actionmap.h"
#include "tred/input/director.h"
#include "tred/input/unitdef.h"
#include "tred/input/unitsetup.h"
#include "tred/input/system.pimpl.h"

#include "tred/handle.h"


namespace input
{


input_system::input_system()
    : m(std::make_unique<InputSystemPiml>())
{
}


input_system::input_system(input_system&& is)
    : m(std::move(is.m))
{
}


result<input_system> load(const config::input_system& config)
{
    input_system system;

    auto actions = load_action_map(config.actions);
    if(actions == false)
    {
        return actions.get_error();
    }
    system.m->actions = std::move(*actions.value);

    auto mapping_list = load_mapping_list(config.keys, system.m->actions);
    if(mapping_list == false)
    {
        return mapping_list.get_error();
    }
    system.m->configs = std::move(*mapping_list.value);


    return system;
}


input_system::~input_system()
{
    // need to clear all players first with 'this' valid
    // otherwise we crash in destructor for active units and the director has
    // been destroyed
    if(m) { m->players.clear(); }
}


void input_system::update_table(PlayerHandle player, table* table, float dt) const
{
    m->players[player]->update_table(table, dt);
}


void input_system::update_player_connections(unit_discovery discovery, platform* platform) const
{
    for(auto& p: m->players)
    {
        p->update_connection_status();

        const bool force_check = discovery == unit_discovery::find_highest && p->is_any_connection_considered_joystick() == false;
        if(p->is_connected() == false || force_check)
        {
            auto setup = unit_setup{};
            for(auto& config_pair: m->configs.configs)
            {
                const auto ignore_this = p->is_connected() == true && config_pair.second->is_any_considered_joystick() == false;
                const auto detected = ignore_this == false && config_pair.second->can_detect(&m->input_director, discovery, &setup, platform);
                if(detected)
                {
                    p->connected_units = config_pair.second->connect(&m->input_director, setup);
                    platform->on_changed_connection(config_pair.first);
                }
            }
        }
    }

    m->input_director.remove_just_pressed();
}


PlayerHandle input_system::add_player() const
{
    const auto r = m->players.create_new_handle();
    m->players[r] = std::make_unique<player>();
    return r;
}


bool input_system::is_connected(PlayerHandle player) const
{
    auto& p = m->players[player];
    return p && p->is_connected();
}


///////////////////////////////////////////////////////////////////////////////


void input_system::on_keyboard_key(keyboard_key key, bool down)
{
    m->input_director.on_keyboard_key(key, down);
}


///////////////////////////////////////////////////////////////////////////////


void input_system::on_mouse_axis(xy_axis axis, float relative_state, float absolute_state)
{
    m->input_director.on_mouse_axis(axis, relative_state, absolute_state);
}


void input_system::on_mouse_wheel(xy_axis axis, float value)
{
    m->input_director.on_mouse_wheel(axis, value);
}


void input_system::on_mouse_button(mouse_button button, bool down)
{
    m->input_director.on_mouse_button(button, down);
}


///////////////////////////////////////////////////////////////////////////////


void input_system::on_joystick_ball(joystick_id joystick, xy_axis type, int ball, float value)
{
    m->input_director.on_joystick_ball(joystick, type, ball, value);
}


void input_system::on_joystick_hat(joystick_id joystick, xy_axis type, int hat, float value)
{
    m->input_director.on_joystick_hat(joystick, type, hat, value);
}


void input_system::on_joystick_button(joystick_id joystick, int button, bool down)
{
    m->input_director.on_joystick_button(joystick, button, down);
}


void input_system::on_joystick_axis(joystick_id joystick, int axis, float value)
{
    m->input_director.on_joystick_axis(joystick, axis, value);
}


void input_system::on_joystick_lost(joystick_id joystick)
{
    m->input_director.on_joystick_lost(joystick);
}


///////////////////////////////////////////////////////////////////////////////


void input_system::on_gamecontroller_button(joystick_id joystick, gamecontroller_button button, float state)
{
    m->input_director.on_gamecontroller_button(joystick, button, state);
}


void input_system::on_gamecontroller_axis(joystick_id joystick, gamecontroller_axis axis, float value)
{
    m->input_director.on_gamecontroller_axis(joystick, axis, value);
}


void input_system::on_gamecontroller_lost(joystick_id joystick)
{
    m->input_director.on_gamecontroller_lost(joystick);
}




}  // namespace input
