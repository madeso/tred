#pragma once

#include "tred/vertex_layout.h"

void
SetShaderProgram(unsigned int new_program, const VertexTypes& types);

bool
IsBoundForShader(const VertexTypes& debug_shader_types);

bool
IsShaderBound(unsigned int program);

void
ClearShaderProgram();