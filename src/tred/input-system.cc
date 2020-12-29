#include "tred/input-system.h"

#include <cassert>
#include <fstream>

#include "fmt/format.h"

#include "tred/input-config.h"
#include "tred/input-director.h"
#include "tred/input-player.h"
#include "tred/input-keyconfig.h"
#include "tred/input-connectedunits.h"
#include "tred/input-keyconfigs.h"
#include "tred/input-actionmap.h"
#include "tred/input-director.h"

#include "tred/handle.h"


namespace input
{


struct InputSystemPiml
{
    using PlayerHandleFunctions = HandleFunctions64<PlayerHandle>;
    using PlayerHandleVector = HandleVector<std::unique_ptr<Player>, PlayerHandleFunctions>;

    InputActionMap actions;
    PlayerHandleVector players;
    KeyConfigs configs;
    InputDirector input_director;
};


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


void InputSystem::SetUnitForPlayer(PlayerHandle player, const std::string& inputname)
{
    auto& config = m->configs.Get(inputname);
    m->players[player]->connected_units = config.Connect(&m->input_director);
}


void InputSystem::UpdateTable(PlayerHandle player, Table* table)
{
    m->players[player]->UpdateTable(table);
}


void InputSystem::Update(float dt)
{
    for (auto& p: m->players)
    {
        p->Update(dt);
    }
}


void InputSystem::OnKeyboardKey(Key key, bool down)
{
    m->input_director.OnKeyboardKey(key, down);
}


void InputSystem::OnMouseAxis(Axis axis, float value)
{
    m->input_director.OnMouseAxis(axis, value);
}


void InputSystem::OnMouseButton(MouseButton button, bool down)
{
    m->input_director.OnMouseButton(button, down);
}


void InputSystem::OnJoystickPov(Axis type, int hat, int joystick, float value)
{
    m->input_director.OnJoystickPov(type, hat, joystick, value);
}


void InputSystem::OnJoystickButton(int button, int joystick, bool down)
{
    m->input_director.OnJoystickButton(button, joystick, down);
}


void InputSystem::OnJoystickAxis(int axis, int joystick, float value)
{
    m->input_director.OnJoystickAxis(axis, joystick, value);
}


PlayerHandle InputSystem::AddPlayer()
{
    const auto r = m->players.Add();
    m->players[r] = std::make_unique<Player>();
    return r;
}


}  // namespace input
