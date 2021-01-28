#pragma once

#include "glm/glm.hpp"

#include <string>
#include <memory>
#include <functional>
#include <optional>

#include "tred/input/unitdiscovery.h"


using RenderFunction = std::function<void (const glm::ivec2& size)>;
using ImguiFunction = std::function<void ()>;
using UpdateFunction = std::function<bool (float)>;

namespace detail
{
struct Window
{
    virtual ~Window() = default;
    virtual void Render() = 0;
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

    void AddWindow(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render);
    void AddWindow(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render, ImguiFunction&& on_imgui);

    virtual input::Platform* GetInputPlatform() = 0;

    virtual void AddWindowImpl(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render, std::optional<ImguiFunction>&& imgui) = 0;
    virtual void Render() = 0;
    virtual void PumpEvents(input::InputSystem* input_system) = 0;
};


std::unique_ptr<Windows> Setup();

int MainLoop(input::UnitDiscovery discovery, std::unique_ptr<Windows>&& windows, input::InputSystem* sdl_input, UpdateFunction&& on_update);
