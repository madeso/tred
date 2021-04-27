#pragma once

#include "glm/glm.hpp"

#include <string>
#include <memory>
#include <functional>
#include <optional>

#include "tred/input/unitdiscovery.h"


using render_function = std::function<void (const glm::ivec2& size)>;
using imgui_function = std::function<void ()>;
using update_function = std::function<bool (float)>;

namespace detail
{
struct window
{
    virtual ~window() = default;
    virtual void render() = 0;
};
}

namespace input
{
struct input_system;
struct platform;
}

struct windows
{
    bool running = true;

    virtual ~windows() = default;

    void add_window(const std::string& title, const glm::ivec2& size, render_function&& on_render);
    void add_window(const std::string& title, const glm::ivec2& size, render_function&& on_render, imgui_function&& on_imgui);

    virtual input::platform* get_input_platform() = 0;

    virtual void add_window_implementation(const std::string& title, const glm::ivec2& size, render_function&& on_render, std::optional<imgui_function>&& imgui) = 0;
    virtual void render() = 0;
    virtual void pump_events(input::input_system* input_system) = 0;
};


std::unique_ptr<windows> setup();

int main_loop(input::unit_discovery discovery, std::unique_ptr<windows>&& windows, input::input_system* sdl_input, update_function&& on_update);
