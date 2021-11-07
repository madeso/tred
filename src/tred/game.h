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
#include "tred/layer2.h"

struct Game
{
    Game() = default;
    virtual ~Game() = default;

    virtual void on_render(const RenderCommand& rc);
    virtual void on_imgui();
    virtual bool on_update(float);

    virtual void on_key(char key, bool down);
    virtual void on_mouse_position(const InputCommand&, const glm::ivec2& position);
    virtual void on_mouse_button(const InputCommand&, input::MouseButton button, bool down);
    virtual void on_mouse_wheel(int scroll);
};

int run_game(const std::string& title, const glm::ivec2& size, bool call_imgui, std::function<std::shared_ptr<Game>()> make_game);
