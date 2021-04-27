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
    struct input_system;
}


namespace input
{


struct table;
struct platform;


enum class PlayerHandle : u64 {};


struct InputSystemPiml;

struct input_system
{
    input_system();
    input_system(input_system&& is);
    ~input_system();

    void update_table(PlayerHandle player, table* table, float dt) const;
    void update_player_connections(unit_discovery discovery, platform* platform) const;

    PlayerHandle add_player() const;
    bool is_connected(PlayerHandle player) const;

    void on_keyboard_key(keyboard_key key, bool down);
    void on_mouse_axis(xy_axis axis, float relative_state, float absolute_state);
    void on_mouse_wheel(xy_axis axis, float value);
    void on_mouse_button(mouse_button button, bool down);

    void on_joystick_ball(joystick_id joystick, xy_axis type, int ball, float value);
    void on_joystick_hat(joystick_id joystick, xy_axis type, int hat, float value);
    void on_joystick_button(joystick_id joystick, int button, bool down);
    void on_joystick_axis(joystick_id joystick, int axis, float value);
    void on_joystick_lost(joystick_id joystick);

    void on_gamecontroller_button(joystick_id joystick, gamecontroller_button button, float state);
    void on_gamecontroller_axis(joystick_id joystick, gamecontroller_axis axis, float value);
    void on_gamecontroller_lost(joystick_id joystick);

    std::unique_ptr<InputSystemPiml> m;
};

result<input_system> load(const config::input_system& config);

}
