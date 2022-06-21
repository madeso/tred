#pragma once

#include "tred/render/shader.h"
#include "tred/render/spritebatch.h"


namespace render
{

struct Render2
{
    Render2();

    ShaderVertexAttributes quad_description;
    CompiledShaderVertexAttributes quad_layout;
    ShaderProgram quad_shader;
    Uniform view_projection_uniform;
    Uniform transform_uniform;
    Uniform texture_uniform;

    SpriteBatch batch;
};

}
