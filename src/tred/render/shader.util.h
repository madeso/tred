#pragma once

#include <unordered_set>

#include "tred/render/vertex_layout.h"


void
set_shader_program(unsigned int new_program, const VertexTypes& types);

bool
is_bound_for_shader(const std::unordered_set<VertexType>& debug_shader_types);

bool
is_shader_bound(unsigned int program);

void
clear_shader_program();
