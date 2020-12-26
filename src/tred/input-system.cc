#include "tred/input-system.h"

#include <cassert>
#include <fstream>

#include "fmt/format.h"

#include "tred/input-config.h"
#include "tred/input-director.h"
#include "tred/input-player.h"
#include "tred/input-keyconfig.h"


namespace input
{

void Load(InputSystem* sys, const config::InputSystem& root)
{
    assert(sys);

    for (const auto& name: root.players)
    {
        sys->AddPlayer(name);
    }
}


InputSystem::InputSystem(const config::InputSystem& config)
    : input(new InputDirector())
{
    Load(&actions, config.actions);
    Load(&configs, config.keys, actions);
    Load(this, config);
}


std::shared_ptr<GlobalToggle> InputSystem::GetAction(const std::string& name)
{
    return actions.GetGlobalToggle(name);
}


void InputSystem::SetUnitForPlayer(const std::string& playerName, const std::string& inputname)
{
    auto res = players.find(playerName);
    if (res == players.end())
    {
        const std::string error = fmt::format("Unable to find player {}", playerName);
        throw error;
    }
    std::shared_ptr<Player> player = res->second;

    auto config = configs.Get(inputname);

    player->units = config->Connect(actions, input.get());
}


void InputSystem::Update(float dt)
{
    actions.Update();

    for (auto p: players)
    {
        p.second->Update(dt);
    }
}


void InputSystem::OnKeyboardKey(Key key, bool down)
{
    input->OnKeyboardKey(key, down);
}


void InputSystem::OnMouseAxis(Axis axis, float value)
{
    input->OnMouseAxis(axis, value);
}


void InputSystem::OnMouseButton(MouseButton button, bool down)
{
    input->OnMouseButton(button, down);
}


void InputSystem::OnJoystickPov(Axis type, int hat, int joystick, float value)
{
    input->OnJoystickPov(type, hat, joystick, value);
}


void InputSystem::OnJoystickButton(int button, int joystick, bool down)
{
    input->OnJoystickButton(button, joystick, down);
}


void InputSystem::OnJoystickAxis(int axis, int joystick, float value)
{
    input->OnJoystickAxis(axis, joystick, value);
}


std::shared_ptr<Player> InputSystem::GetPlayer(const std::string& name)
{
    auto found = players.find(name);
    
    if (found == players.end())
    {
        const std::string error = fmt::format("Unable to find player {}", name);
        throw error;
    }

    assert(found->second);
    return found->second;
}


void InputSystem::AddPlayer(const std::string& name)
{
    std::shared_ptr<Player> p(new Player());
    players.insert(std::make_pair(name, p));
}


}  // namespace input
