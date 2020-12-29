#include "tred/input-system.h"

#include <cassert>
#include <fstream>

#include "fmt/format.h"

#include "tred/input-config.h"
#include "tred/input-director.h"
#include "tred/input-player.h"
#include "tred/input-keyconfig.h"
#include "tred/input-connectedunits.h"


namespace input
{
InputSystem::InputSystem(const config::InputSystem& config)
{
    Load(&actions, config.actions);
    Load(&configs, config.keys, actions);
}


InputSystem::~InputSystem()
{
    // need to clear all players first with 'this' valid
    // otherwise we crash in destructor for active units and the director has
    // been destroyed
    players.clear();
}


void InputSystem::SetUnitForPlayer(Player* player, const std::string& inputname)
{
    auto& config = configs.Get(inputname);
    player->connected_units = config.Connect(&input_director);
}


void InputSystem::Update(float dt)
{
    for (auto& p: players)
    {
        p->Update(dt);
    }
}


void InputSystem::OnKeyboardKey(Key key, bool down)
{
    input_director.OnKeyboardKey(key, down);
}


void InputSystem::OnMouseAxis(Axis axis, float value)
{
    input_director.OnMouseAxis(axis, value);
}


void InputSystem::OnMouseButton(MouseButton button, bool down)
{
    input_director.OnMouseButton(button, down);
}


void InputSystem::OnJoystickPov(Axis type, int hat, int joystick, float value)
{
    input_director.OnJoystickPov(type, hat, joystick, value);
}


void InputSystem::OnJoystickButton(int button, int joystick, bool down)
{
    input_director.OnJoystickButton(button, joystick, down);
}


void InputSystem::OnJoystickAxis(int axis, int joystick, float value)
{
    input_director.OnJoystickAxis(axis, joystick, value);
}


Player* InputSystem::AddPlayer()
{
    auto p = std::make_unique<Player>();
    auto* r = p.get();
    players.emplace_back(std::move(p));
    return r;
}


}  // namespace input
