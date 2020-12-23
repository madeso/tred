#pragma once

#include "glm/glm.hpp"

#include <string>
#include <memory>
#include <functional>
#include <optional>

using RenderFunction = std::function<void ()>;
using UpdateFunction = std::function<bool ()>;

struct Input;

namespace detail
{
struct Window
{
    virtual ~Window() = default;
    virtual void Render() = 0;
};
}

struct Windows
{
    bool running = true;

    virtual ~Windows() = default;

    void AddWindow(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render);
    void AddWindow(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render, RenderFunction&& on_imgui);

    virtual void AddWindowImpl(const std::string& title, const glm::ivec2& size, RenderFunction&& on_render, std::optional<RenderFunction>&& imgui) = 0;
    virtual void Render() = 0;
    virtual void PumpEvents(Input* input) = 0;
};


std::unique_ptr<Windows> Setup();

int MainLoop(std::unique_ptr<Windows>&& windows, Input* input, UpdateFunction&& on_update);
