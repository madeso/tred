#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "tred/types.h"

#include "tred/input/key.h"
#include "tred/input/axis.h"
#include "tred/input/platform.h"


namespace input::config
{
    struct InputSystem;
}


namespace input
{


struct Table;


enum class PlayerHandle : u64 {};


struct InputSystemPiml;

struct InputSystem
{
    explicit InputSystem(const config::InputSystem& config);
    ~InputSystem();

    void SetUnitForPlayer(PlayerHandle player, const std::string& inputName);
    void UpdateTable(PlayerHandle player, Table* table);

    void OnKeyboardKey(Key key, bool down);
    void OnMouseAxis(Axis axis, float value);
    void OnMouseWheel(Axis axis, float value);
    void OnMouseButton(MouseButton button, bool down);

    void OnJoystickBall(JoystickId joystick, Axis type, int ball, float value);
    void OnJoystickHat(JoystickId joystick, Axis type, int hat, float value);
    void OnJoystickButton(JoystickId joystick, int button, bool down);
    void OnJoystickAxis(JoystickId joystick, int axis, float value);

    PlayerHandle AddPlayer();

    std::unique_ptr<InputSystemPiml> m;
};

}  // namespace input
