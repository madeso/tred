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

#include "tred/handle.h"


namespace input
{


struct InputSystemPiml
{
    using PlayerHandleFunctions = HandleFunctions64<PlayerHandle>;
    using PlayerHandleVector = HandleVector<std::unique_ptr<Player>, PlayerHandleFunctions>;

    InputActionMap actions;
    PlayerHandleVector players;
    MappingList configs;
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
    auto setup = UnitSetup{};
    m->players[player]->connected_units = config.Connect(&m->input_director, setup);
}


void InputSystem::UpdateTable(PlayerHandle player, Table* table)
{
    m->players[player]->UpdateTable(table);
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




PlayerHandle InputSystem::AddPlayer()
{
    const auto r = m->players.Add();
    m->players[r] = std::make_unique<Player>();
    return r;
}


}  // namespace input
