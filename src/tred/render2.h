#pragma once

#include "tred/shader.h"
#include "tred/spritebatch.h"


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
