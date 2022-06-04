#pragma once


#include "tred/dependency_opengl.h"

// use std::optional and bool?

struct OpenglStates
{
    std::optional<bool> cull_face;
    std::optional<bool> blending;
    std::optional<bool> depth_test;

    OpenglStates& set_cull_face(std::optional<bool> t) { cull_face = t; return *this; }
    OpenglStates& set_blending(std::optional<bool> t) { blending = t; return *this; }
    OpenglStates& set_depth_test(std::optional<bool> t) { depth_test = t; return *this; }
};


void apply(OpenglStates* current_states, const OpenglStates& new_states);
