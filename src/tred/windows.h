#pragma once

#include "tred/dependency_glm.h"

#include <string>
#include <memory>
#include <functional>
#include <optional>

#include "tred/types.h"
#include "tred/input/unitdiscovery.h"

struct RenderCommand;

using render_function = std::function<void (const RenderCommand&)>;
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

enum WindowHandle : u64 {};

struct Windows
{
    bool running = true;

    virtual ~Windows() = default;

    virtual void set_render(WindowHandle window, render_function&& on_render) = 0;
    virtual void set_imgui(imgui_function&& on_render) = 0;

    virtual input::Platform* get_input_platform() = 0;

    virtual WindowHandle add_window(const std::string& title, const glm::ivec2& size) = 0;
    virtual void render() = 0;
    virtual void pump_events(input::InputSystem* input_system) = 0;
};


std::unique_ptr<Windows> setup();

int main_loop(input::unit_discovery discovery, std::unique_ptr<Windows>&& windows, input::InputSystem* sdl_input, update_function&& on_update);
