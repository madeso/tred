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

struct InputSystemBase
{
    virtual ~InputSystemBase() = default;

    virtual void update_player_connections(unit_discovery discovery, Platform* platform) const = 0;
    virtual bool is_mouse_connected() const = 0;

    virtual void on_keyboard_key(KeyboardKey key, bool down) = 0;
    virtual void on_mouse_axis(Axis2 axis, float relative_state, float absolute_state) = 0;
    virtual void on_mouse_wheel(Axis2 axis, float value) = 0;
    virtual void on_mouse_button(MouseButton button, bool down) = 0;

    virtual void on_joystick_ball(JoystickId joystick, Axis2 type, int ball, float value) = 0;
    virtual void on_joystick_hat(JoystickId joystick, Axis2 type, int hat, float value) = 0;
    virtual void on_joystick_button(JoystickId joystick, int button, bool down) = 0;
    virtual void on_joystick_axis(JoystickId joystick, int axis, float value) = 0;
    virtual void on_joystick_lost(JoystickId joystick) = 0;

    virtual void on_gamecontroller_button(JoystickId joystick, GamecontrollerButton button, float state) = 0;
    virtual void on_gamecontroller_axis(JoystickId joystick, GamecontrollerAxis axis, float value) = 0;
    virtual void on_gamecontroller_lost(JoystickId joystick) = 0;
};

struct InputSystem : InputSystemBase
{
    InputSystem();
    InputSystem(InputSystem&& is);
    ~InputSystem();

    void update_table(PlayerHandle player, Table* table, float dt) const;
    void update_player_connections(unit_discovery discovery, Platform* platform) const override;

    PlayerHandle add_player() const;
    bool is_connected(PlayerHandle player) const;

    bool is_mouse_connected() const override;

    void on_keyboard_key(KeyboardKey key, bool down) override;
    void on_mouse_axis(Axis2 axis, float relative_state, float absolute_state) override;
    void on_mouse_wheel(Axis2 axis, float value) override;
    void on_mouse_button(MouseButton button, bool down) override;

    void on_joystick_ball(JoystickId joystick, Axis2 type, int ball, float value) override;
    void on_joystick_hat(JoystickId joystick, Axis2 type, int hat, float value) override;
    void on_joystick_button(JoystickId joystick, int button, bool down) override;
    void on_joystick_axis(JoystickId joystick, int axis, float value) override;
    void on_joystick_lost(JoystickId joystick) override;

    void on_gamecontroller_button(JoystickId joystick, GamecontrollerButton button, float state) override;
    void on_gamecontroller_axis(JoystickId joystick, GamecontrollerAxis axis, float value) override;
    void on_gamecontroller_lost(JoystickId joystick) override;

    std::unique_ptr<InputSystemPiml> m;
};

Result<InputSystem> load(const config::InputSystem& config);

}
