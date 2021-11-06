#pragma once

#include "tred/dependency_glm.h"

#include <string>
#include <memory>
#include <functional>
#include <optional>

#include "tred/shader.h"
#include "tred/vertex_layout.h"
#include "tred/rect.h"
#include "tred/types.h"
#include "tred/texture.h"
#include "tred/spritebatch.h"
#include "tred/input/key.h"
#include "tred/render2.h"


struct Command2
{
    glm::ivec2 size;
};

struct RenderCommand2
{
    Render2* render;
    glm::ivec2 size;
};

struct Layer2
{
    Rectf viewport_aabb_in_worldspace;
    Recti screen;

    glm::vec2 mouse_to_world(const glm::vec2& p) const;
};

struct RenderLayer2 : Layer2
{
    SpriteBatch* batch;

    RenderLayer2(Layer2&& l, SpriteBatch* batch);

    ~RenderLayer2();
};

enum class ViewportStyle { black_bars, extended};

struct LayoutData
{
    ViewportStyle style;
    float requested_width;
    float requested_height;
    const glm::mat4 camera;
};

RenderLayer2 with_layer(const RenderCommand2& rc, const LayoutData& ld);
Layer2 with_layer(const Command2& rc, const LayoutData& ld);

struct Game
{
    Game() = default;
    virtual ~Game() = default;

    virtual void on_render(const RenderCommand2& rc);
    virtual void on_imgui();
    virtual bool on_update(float);

    virtual void on_key(char key, bool down);
    virtual void on_mouse_position(const Command2&, const glm::ivec2& position);
    virtual void on_mouse_button(const Command2&, input::MouseButton button, bool down);
    virtual void on_mouse_wheel(int scroll);
};

int run_game(const std::string& title, const glm::ivec2& size, bool call_imgui, std::function<std::shared_ptr<Game>()> make_game);
