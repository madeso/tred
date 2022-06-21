#pragma once

namespace render
{

struct OpenglStates;

void opengl_setup(OpenglStates* states);
void opengl_set2d(OpenglStates* states);
void opengl_set3d(OpenglStates* states);

}
