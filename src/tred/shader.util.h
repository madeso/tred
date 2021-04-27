#pragma once

#include "tred/vertex_layout.h"

void
set_shader_program(unsigned int new_program, const vertex_types& types);

bool
is_bound_for_shader(const vertex_types& debug_shader_types);

bool
is_shader_bound(unsigned int program);

void
clear_shader_program();
