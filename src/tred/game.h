#pragma once

#include "glm/glm.hpp"

#include <string>
#include <memory>
#include <functional>

#include "tred/shader.h"
#include "tred/vertex_layout.h"
#include "tred/rect.h"
#include "tred/types.h"

struct Texture;
struct Render2;

struct SpriteBatch
{
    static constexpr int max_quads = 100;

    std::vector<float> data;
    int quads = 0;
    Texture* current_texture = nullptr;
    u32 va;
    u32 vb;
    u32 ib;
    Render2* render;

    SpriteBatch(Shader* shader, Render2* r);

    void add_vertex(const glm::vec2& position, const glm::vec4& color, const glm::vec2& uv);
    void add_vertex(const glm::vec3& position, const glm::vec4& color, const glm::vec2& uv);

    void quad(Texture* texture, const rect& scr, const rect& tex, const glm::vec4& tint = glm::vec4(1.0f));

    void submit();

};

struct Render2
{
    Render2();
    ~Render2();

    VertexLayoutDescription quad_description;
    CompiledVertexLayout quad_layout;
    Shader quad_shader;
    Uniform view_projection_uniform;
    Uniform transform_uniform;
    Uniform texture_uniform;

    SpriteBatch batch;
};

struct RenderCommand2
{
    Render2* render;
    glm::ivec2 size;
};

struct Game
{
    Game() = default;
    virtual ~Game() = default;

    virtual void OnRender(const RenderCommand2& rc);
    virtual void OnImgui();
    virtual bool OnUpdate(float);

    virtual void OnKey(char key, bool down);
    virtual void OnMouseMotion(const glm::ivec2& position);
    virtual void OnMouseButton(int button, bool down);
    virtual void OnMouseWheel(int scroll);
};

int Run(const std::string& title, const glm::ivec2& size, bool call_imgui, std::function<std::shared_ptr<Game>()> make_game);
