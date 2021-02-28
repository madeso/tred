#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "tred/types.h"
#include "tred/result.h"

#include "tred/input/key.h"
#include "tred/input/axis.h"
#include "tred/input/platform.h"
#include "tred/input/unitdiscovery.h"


namespace input::config
{
    struct InputSystem;
}


namespace input
{


struct Table;
struct Platform;


enum class PlayerHandle : u64 {};


struct InputSystemPiml;

struct InputSystem
{
    InputSystem();
    InputSystem(InputSystem&& is);
    ~InputSystem();

    void UpdateTable(PlayerHandle player, Table* table, float dt);
    void UpdatePlayerConnections(UnitDiscovery discovery, Platform* platform);

    PlayerHandle AddPlayer();
    bool IsConnected(PlayerHandle player);

    void OnKeyboardKey(Key key, bool down);
    void OnMouseAxis(Axis axis, float relative_state, float absolute_state);
    void OnMouseWheel(Axis axis, float value);
    void OnMouseButton(MouseButton button, bool down);

    void OnJoystickBall(JoystickId joystick, Axis type, int ball, float value);
    void OnJoystickHat(JoystickId joystick, Axis type, int hat, float value);
    void OnJoystickButton(JoystickId joystick, int button, bool down);
    void OnJoystickAxis(JoystickId joystick, int axis, float value);
    void OnJoystickLost(JoystickId joystick);

    void OnGamecontrollerButton(JoystickId joystick, GamecontrollerButton button, float state);
    void OnGamecontrollerAxis(JoystickId joystick, GamecontrollerAxis axis, float value);
    void OnGamecontrollerLost(JoystickId joystick);

    std::unique_ptr<InputSystemPiml> m;
};

Result<InputSystem> Load(const config::InputSystem& config);

}  // namespace input
