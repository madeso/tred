#pragma once

#include "tred/dependency_glm.h"
#include "tred/rect.h"

struct Render2;
struct SpriteBatch;


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
