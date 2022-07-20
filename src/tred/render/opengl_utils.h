#pragma once

namespace render
{

struct OpenglStates
{
    std::optional<bool> cull_face;
    std::optional<bool> blending;
    std::optional<bool> depth_test;
};

void opengl_setup(OpenglStates* states);
void opengl_set2d(OpenglStates* states);
void opengl_set3d(OpenglStates* states);

}
