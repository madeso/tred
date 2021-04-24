#pragma once

#include "glm/glm.hpp"

#include <string>
#include <memory>
#include <functional>

#include "tred/shader.h"
#include "tred/vertex_layout.h"

struct Game
{
    VertexLayoutDescription quad_description;
    CompiledVertexLayout quad_layout;
    Shader quad_shader;
    Uniform view_projection_uniform;
    Uniform transform_uniform;
    Uniform texture_uniform;

    Game();
    virtual ~Game();

    virtual void OnRender(const glm::ivec2& size);
    virtual void OnImgui();
    virtual bool OnUpdate(float);

    virtual void OnKey(char key, bool down);
    virtual void OnMouseMotion(const glm::ivec2& position);
    virtual void OnMouseButton(int button, bool down);
    virtual void OnMouseWheel(int scroll);
};

int Run(const std::string& title, const glm::ivec2& size, bool call_imgui, std::function<std::shared_ptr<Game>()> make_game);
