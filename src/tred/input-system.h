#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "tred/input-key.h"
#include "tred/input-axis.h"
#include "tred/input-keyconfigs.h"
#include "tred/input-actionmap.h"
#include "tred/input-director.h"


namespace input
{

namespace config
{
    struct InputSystem;
}

struct Table;
struct Player;

struct InputSystem
{
    explicit InputSystem(const config::InputSystem& config);
    ~InputSystem();

    void SetUnitForPlayer(std::shared_ptr<Player> player, const std::string& inputName);

    void Update(float dt);

    void OnKeyboardKey(Key key, bool down);
    void OnMouseAxis(Axis axis, float value);
    void OnMouseButton(MouseButton button, bool down);

    void OnJoystickPov(Axis type, int hat, int joystick, float value);
    void OnJoystickButton(int button, int joystick, bool down);
    void OnJoystickAxis(int axis, int joystick, float value);

    std::shared_ptr<Player> AddPlayer();

    InputActionMap actions;
    std::vector<std::shared_ptr<Player>> players;
    KeyConfigs configs;
    InputDirector input_director;
};

}  // namespace input
