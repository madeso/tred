#pragma once

namespace render
{

void clear_stencil_and_depth();

// todo(Gustav): make theese part of material
void set_poly_mode_to_fill();
void set_poly_mode_to_line();
void set_poly_mode_to_point();

}
