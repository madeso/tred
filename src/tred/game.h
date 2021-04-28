#pragma once

#include "glm/glm.hpp"

#include <string>
#include <memory>
#include <functional>
#include <optional>

#include "tred/shader.h"
#include "tred/vertex_layout.h"
#include "tred/rect.h"
#include "tred/types.h"
#include "tred/texture.h"
#include "tred/input/key.h"

struct texture;
struct render2;

struct vertex2
{
    glm::vec3 position;
    glm::vec4 color;
    glm::vec2 texturecoord;
};

struct sprite_batch
{
    static constexpr int max_quads = 100;

    std::vector<float> data;
    int quads = 0;
    texture* current_texture = nullptr;
    u32 va;
    u32 vb;
    u32 ib;
    render2* render;
    texture white_texture;

    sprite_batch(shader* shader, render2* r);

    void quad(std::optional<texture*> texture, const vertex2& v0, const vertex2& v1, const vertex2& v2, const vertex2& v3);
    void quad(std::optional<texture*> texture, const rect& scr, const std::optional<rect>& texturecoord, const glm::vec4& tint = glm::vec4(1.0f));
    void quad(std::optional<texture*> texture, const rect& scr, const recti& texturecoord, const glm::vec4& tint = glm::vec4(1.0f));

    void submit();

};

struct render2
{
    render2();
    ~render2();

    vertex_layout_description quad_description;
    compiled_vertex_layout quad_layout;
    shader quad_shader;
    uniform view_projection_uniform;
    uniform transform_uniform;
    uniform texture_uniform;

    sprite_batch batch;
};

struct command2
{
    glm::ivec2 size;
};

struct render_command2
{
    render2* render;
    glm::ivec2 size;
};

struct layer2
{
    rect viewport_aabb_in_worldspace;
    recti screen;

    glm::vec2 mouse_to_world(const glm::vec2& p) const;
};

struct render_layer2 : layer2
{
    sprite_batch* batch;

    render_layer2(layer2&& l, sprite_batch* batch);

    ~render_layer2();
};

render_layer2 with_layer_fit_with_bars(const render_command2& rc, float requested_width, float requested_height, const glm::mat4 camera);
render_layer2 with_layer_extended(const render_command2& rc, float requested_width, float requested_height, const glm::mat4 camera);

layer2 with_layer_fit_with_bars(const command2& rc, float requested_width, float requested_height, const glm::mat4 camera);
layer2 with_layer_extended(const command2& rc, float requested_width, float requested_height, const glm::mat4 camera);

struct game
{
    game() = default;
    virtual ~game() = default;

    virtual void on_render(const render_command2& rc);
    virtual void on_imgui();
    virtual bool on_update(float);

    virtual void on_key(char key, bool down);
    virtual void on_mouse_position(const glm::ivec2& position);
    virtual void on_mouse_button(const command2&, input::mouse_button button, bool down);
    virtual void on_mouse_wheel(int scroll);
};

int run_game(const std::string& title, const glm::ivec2& size, bool call_imgui, std::function<std::shared_ptr<game>()> make_game);
