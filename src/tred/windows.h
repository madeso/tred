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
struct Window
{
    virtual ~Window() = default;
    virtual void render() = 0;
};
}

namespace input
{
struct InputSystem;
struct Platform;
}

struct Windows
{
    bool running = true;

    virtual ~Windows() = default;

    void add_window(const std::string& title, const glm::ivec2& size, render_function&& on_render);
    void add_window(const std::string& title, const glm::ivec2& size, render_function&& on_render, imgui_function&& on_imgui);

    virtual input::Platform* get_input_platform() = 0;

    virtual void add_window_implementation(const std::string& title, const glm::ivec2& size, render_function&& on_render, std::optional<imgui_function>&& imgui) = 0;
    virtual void render() = 0;
    virtual void pump_events(input::InputSystem* input_system) = 0;
};


std::unique_ptr<Windows> setup();

int main_loop(input::unit_discovery discovery, std::unique_ptr<Windows>&& windows, input::InputSystem* sdl_input, update_function&& on_update);
