#pragma once

#include <vector>
#include <map>
#include <string>
#include <memory>

#include "tred/types.h"

#include "tred/input/key.h"
#include "tred/input/axis.h"


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

    void Update(float dt);

    void OnKeyboardKey(Key key, bool down);
    void OnMouseAxis(Axis axis, float value);
    void OnMouseButton(MouseButton button, bool down);

    void OnJoystickPov(Axis type, int hat, int joystick, float value);
    void OnJoystickButton(int button, int joystick, bool down);
    void OnJoystickAxis(int axis, int joystick, float value);

    PlayerHandle AddPlayer();

    std::unique_ptr<InputSystemPiml> m;
};

}  // namespace input
