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


InputSystem::InputSystem(const config::InputSystem& config)
    : m(std::make_unique<InputSystemPiml>())
{
    Load(&m->actions, config.actions);
    Load(&m->configs, config.keys, m->actions);
}


InputSystem::~InputSystem()
{
    // need to clear all players first with 'this' valid
    // otherwise we crash in destructor for active units and the director has
    // been destroyed
    m->players.Clear();
}


void InputSystem::UpdateTable(PlayerHandle player, Table* table, float dt)
{
    m->players[player]->UpdateTable(table, dt);
}


void InputSystem::UpdatePlayerConnections(UnitDiscovery discovery, Platform* platform)
{
    for(auto& p: m->players)
    {
        p->UpdateConnectionStatus();

        const bool force_check = discovery == UnitDiscovery::FindHighest && p->IsAnyConnectionConsideredJoystick() == false;
        if(p->IsConnected() == false || force_check)
        {
            auto setup = UnitSetup{};
            for(auto& config_pair: m->configs.configs)
            {
                const auto ignore_this = p->IsConnected() == true && config_pair.second->IsAnyConsideredJoystick() == false;
                const auto detected = ignore_this == false && config_pair.second->CanDetect(&m->input_director, discovery, &setup, platform);
                if(detected)
                {
                    p->connected_units = config_pair.second->Connect(&m->input_director, setup);
                }
            }
        }
    }

    m->input_director.RemoveJustPressed();
}


void InputSystem::OnKeyboardKey(Key key, bool down)
{
    m->input_director.OnKeyboardKey(key, down);
}


void InputSystem::OnMouseAxis(Axis axis, float value)
{
    m->input_director.OnMouseAxis(axis, value);
}


void InputSystem::OnMouseWheel(Axis axis, float value)
{
    m->input_director.OnMouseWheel(axis, value);
}


void InputSystem::OnMouseButton(MouseButton button, bool down)
{
    m->input_director.OnMouseButton(button, down);
}


void InputSystem::OnJoystickBall(JoystickId joystick, Axis type, int ball, float value)
{
    m->input_director.OnJoystickBall(joystick, type, ball, value);
}


void InputSystem::OnJoystickHat(JoystickId joystick, Axis type, int hat, float value)
{
    m->input_director.OnJoystickHat(joystick, type, hat, value);
}


void InputSystem::OnJoystickButton(JoystickId joystick, int button, bool down)
{
    m->input_director.OnJoystickButton(joystick, button, down);
}


void InputSystem::OnJoystickAxis(JoystickId joystick, int axis, float value)
{
    m->input_director.OnJoystickAxis(joystick, axis, value);
}


void InputSystem::OnJoystickLost(JoystickId joystick)
{
    m->input_director.OnJoystickLost(joystick);
}


PlayerHandle InputSystem::AddPlayer()
{
    const auto r = m->players.Add();
    m->players[r] = std::make_unique<Player>();
    return r;
}


bool InputSystem::IsConnected(PlayerHandle player)
{
    auto& p = m->players[player];
    return p && p->IsConnected();
}


}  // namespace input
