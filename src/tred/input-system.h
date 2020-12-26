#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "tred/input-key.h"
#include "tred/input-axis.h"
#include "tred/input-keyconfigs.h"
#include "tred/input-actionmap.h"


struct Table;


namespace input
{

namespace config
{
    struct InputSystem;
}

struct Player;
struct GlobalToggle;
struct InputDirector;

struct InputSystem
{
    explicit InputSystem(const config::InputSystem& config);

    std::shared_ptr<GlobalToggle> GetAction(const std::string& name);
    void SetUnitForPlayer(const std::string& playerName, const std::string& inputName);

    void Update(float dt);

    void OnKeyboardKey(Key key, bool down);
    void OnMouseAxis(Axis axis, float value);
    void OnMouseButton(MouseButton button, bool down);

    void OnJoystickPov(Axis type, int hat, int joystick, float value);
    void OnJoystickButton(int button, int joystick, bool down);
    void OnJoystickAxis(int axis, int joystick, float value);

    std::shared_ptr<Player> GetPlayer(const std::string& name);
    void AddPlayer(const std::string& name);

    InputActionMap actions;
    std::map<std::string, std::shared_ptr<Player>> players;
    KeyConfigs configs;
    std::unique_ptr<InputDirector> input;
};

}  // namespace input
